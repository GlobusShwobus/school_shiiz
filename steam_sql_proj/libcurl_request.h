#pragma once

#include <string>
#include "Logger.h"
#include <span>
#include "Sequence.h"
#include <curl/curl.h>
#include "WebRequestHandle.h"

namespace badSQL
{
	static std::size_t call_back(void* content, std::size_t size, std::size_t nmemb, void* clientp)
	{
		size_t realSize = size * nmemb;

		auto* handle = static_cast<WebRequestHandle*>(clientp);
		auto* bytes = static_cast<const char*>(content);

		handle->data.insert_back(
			bytes,
			bytes + realSize
		);

		return realSize;
	}

	// returns true on success, false on failure
	// on failure call Logger::last_error for more info
	bool validate_certificate(std::string_view path)
	{
		CURL* curl = curl_easy_init();

		if (!curl) {
			return false;
		}

		curl_easy_setopt(curl, CURLOPT_CAINFO, path);

		//test on local host
		curl_easy_setopt(curl, CURLOPT_URL, "https://localhost");
		curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);

		CURLcode response = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		return response == CURLE_OK || response == CURLE_COULDNT_CONNECT;
	}

	WebRequestHandle request_data(std::string_view url, std::string_view certificate)
	{
		WebRequestHandle handle;
		auto& logger = Logger::instance();

		CURL* curl = curl_easy_init();

		if (!curl) {
			logger.add_error("failed to init libcurl");
			return handle;
		}

		struct CurlCleanup {
			CURL* c;
			~CurlCleanup() {
				curl_easy_cleanup(c);
			}
		}cleanup{ curl };


		auto setopt = [&](CURLoption opt, auto value) {
			CURLcode rc = curl_easy_setopt(curl, opt, value);
			if (rc != CURLE_OK) {
				logger.add_error(std::string("curl_easy_setopt fail ") + curl_easy_strerror(rc));
				return false;
			}
			return true;
			};
		//NOTE: using std::string_view and C interface must give .data()
		if (!setopt(CURLOPT_URL, url.data()) ||
			!setopt(CURLOPT_WRITEFUNCTION, call_back) ||
			!setopt(CURLOPT_WRITEDATA, &handle) ||
			!setopt(CURLOPT_CAINFO, certificate.data()))
		{
			return handle;
		}

		CURLcode rc = curl_easy_perform(curl);

		if (rc != CURLE_OK) {
			logger.add_error(std::string("curl_easy_perform failed: ") + curl_easy_strerror(rc));
			return handle;
		}

		handle.is_good = true;

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &handle.httpcode);
		curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &handle.contentType);

		return handle;
	}

	struct CurlEasyCleanup {
		void operator()(CURL* c) {
			if (c) {
				curl_easy_cleanup(c);
			}
		}
	};

	struct CurlEasyRAII {
		std::unique_ptr<CURL, CurlEasyCleanup> curl;

		CurlEasyRAII()
		{
			curl.reset(curl_easy_init());
		}
	};

	struct CurlMultiCleanup {
		void operator()(CURLM* m) {
			if (m) {
				curl_multi_cleanup(m);
			}
		}
	};

	struct CurlMultiRAII {

		std::unique_ptr<CURLM, CurlMultiCleanup> multi;
		CurlMultiRAII()
		{
			multi.reset(curl_multi_init());
		}
	};

	struct CurlRequest {
		CurlEasyRAII curl;
		WebRequestHandle handle;

		CURLcode setup(const std::string& url, std::string_view cert)
		{
			CURL* c = curl.curl.get();
			CURLcode rc;

			//basic setup
			rc = curl_easy_setopt(c, CURLOPT_URL, url.data());
			if (rc != CURLE_OK) return rc;

			rc = curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, call_back);
			if (rc != CURLE_OK) return rc;

			rc = curl_easy_setopt(c, CURLOPT_WRITEDATA, &handle);
			if (rc != CURLE_OK) return rc;

			// Store pointer to THIS object (stable storage)
			rc = curl_easy_setopt(c, CURLOPT_PRIVATE, this);
			if (rc != CURLE_OK) return rc;

			//essential options
			curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(c, CURLOPT_MAXREDIRS, 5L);
			curl_easy_setopt(c, CURLOPT_TIMEOUT, 30L);
			curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 10L);
			curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 1L);
			curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 2L);

			if (!cert.empty()) {
				rc = curl_easy_setopt(c, CURLOPT_CAINFO, cert.data());
				if (rc != CURLE_OK) return rc;
			}

			return CURLE_OK;
		}
	};

	Sequence<WebRequestHandle> multi_request_data(std::span<const std::string> urls, std::string_view certificate)
	{
		Logger& logger = Logger::instance();
		CurlMultiRAII multiRAII;
		CURLM* multi = multiRAII.multi.get();

		// if failed init
		if (!multi) {
			logger.add_error("curl_multi_init failed");
			return {};
		}
		//phase 1: set up handles, and curl objects and push them to multihandle
	
		Sequence<CurlRequest> requests;
		requests.set_capacity(urls.size());

		for (auto& url : urls) {
			requests.emplace_back();
			auto& req = requests.back();

			CURLcode result = req.setup(url, certificate);

			if (result != CURLE_OK) {
				logger.add_error(std::string("FAIL setup: ") + curl_easy_strerror(result) + " [" + std::string(url) + "]");
				requests.pop_back();
				continue;
			}
			curl_multi_add_handle(multi, req.curl.curl.get());//lol
		}

		//phase 2: preform multi curl
		int still_running = 1;
		while (still_running) {
			CURLMcode mrc = curl_multi_perform(multi, &still_running);
			if (mrc != CURLM_OK && mrc != CURLM_CALL_MULTI_PERFORM) {
				logger.add_error(std::string("curl_multi_perform failed: ") + curl_multi_strerror(mrc));
				break;//if the CURLM itself broke not individual call, at that point nothing left to do
			}

			if (still_running) {
				//wait for activity 1000 ms
				mrc = curl_multi_poll(multi, nullptr, 0, 1000, nullptr);

				if (mrc != CURLM_OK) {
					logger.add_error(std::string("curl_multi_poll failed: ") + curl_multi_strerror(mrc));
				}
			}
		}

		//phase 3: collect results

		CURLMsg* msg = nullptr;
		int msgs_left = 0;
		Sequence<WebRequestHandle> results;

		while ((msg = curl_multi_info_read(multi, &msgs_left))) {

			if (msg->msg == CURLMSG_DONE) {
				CurlRequest* req = nullptr;

				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &req);
				if (req) {
					//http code
					curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &req->handle.httpcode);
					
					//content type
					char* content_type = nullptr;
					curl_easy_getinfo(msg->easy_handle, CURLINFO_CONTENT_TYPE, &content_type);

					if (content_type) {
						req->handle.contentType = content_type;
					}

					req->handle.is_good = (msg->data.result == CURLE_OK &&
						req->handle.httpcode >= 200 &&
						req->handle.httpcode < 300);

					results.push_back(std::move(req->handle));
				}
			}
			curl_multi_remove_handle(multi, msg->easy_handle);
		}

		return results;
	}

	/*
	NOTE:: CURL GLOBAL INIT DONE ONCE PER PROGRAM, NOT FUNCTION CALL

		CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
		if (result != CURLE_OK) {
			logger.add_error("Global init fail"+ std::string(curl_easy_strerror(result)));
			return handles;
		}

		curl_global_cleanup();
	*/

}