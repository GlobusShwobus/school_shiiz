#pragma once

#include <string>
#include <vector>
#include "Logger.h"

#include <curl/curl.h>

namespace badSQL
{
	//moshi moshi?
	struct WebRequestHandle {
		Sequence<char> data;
		long httpcode = -1;
		bool is_good = false;
		std::string contentType;
	};


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


	Sequence<WebRequestHandle> multi_request_data(std::span<std::string_view> urls, std::string_view certificate)
	{
		Sequence<WebRequestHandle> handles;
		auto& logger = Logger::instance();

		//phase 0: try to global init
		CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
		if (result != CURLE_OK) {
			logger.add_error("Global init fail"+ std::string(curl_easy_strerror(result)));
			return handles;
		}

		//phase 1: set up all the individual curl handles
		struct CURL_ARRAY {
			Sequence<CURL*> curlarry;

			~CURL_ARRAY() {
				for (auto* c : curlarry) {
					curl_easy_cleanup(c);
				}
			}
		}curlarry;

		auto& curls = curlarry.curlarry;
		curls.set_capacity(urls.size());
		handles.set_capacity(urls.size());

		//TODO:: THIS STEP SHOULD BE DONE BETTER, NOT SURE HOW YET THO. I SHOULD BE ABLE TO SET PER CASE OPTIONS + REPETITION
		for (const auto& url:urls) {
			CURL* c = curl_easy_init();
			handles.emplace_back();
			auto& handle = handles.back();
			if (!c) {
				logger.add_error("failed to init libcurl");
				continue;
			}

			CURLcode op1 = curl_easy_setopt(c, CURLOPT_URL, url.data());
			if (!op1) {
				logger.add_error(std::string("curl_easy_setopt fail") + curl_easy_strerror(op1));
				curl_easy_cleanup(c);
				continue;
			}

			CURLcode op2 = curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, call_back);
			if (!op2) {
				logger.add_error(std::string("curl_easy_setopt fail") + curl_easy_strerror(op2));
				curl_easy_cleanup(c);
				continue;
			}

			CURLcode op3 = curl_easy_setopt(c, CURLOPT_CAINFO, certificate.data());
			if (!op3) {
				logger.add_error(std::string("curl_easy_setopt fail") + curl_easy_strerror(op3));
				curl_easy_cleanup(c);
				continue;
			}

			CURLcode op3 = curl_easy_setopt(c, CURLOPT_WRITEDATA, &handle);
			if (!op3) {
				logger.add_error(std::string("curl_easy_setopt fail") + curl_easy_strerror(op3));
				curl_easy_cleanup(c);
				handles.pop_back();
				continue;
			}
		}

		//phase 3: set up multi handle
		CURLM* multi = curl_multi_init();

		if (multi) {
			int still_running = 1;

			CURLMsg* msg;
			int msgs_left;

			for (auto* c : curls) {
				curl_multi_add_handle(multi, c);
			}

			while (still_running) {
				CURLMcode mresult = curl_multi_perform(multi, &still_running);
				if (still_running)
					/* wait for activity, timeout or "nothing" */
					mresult = curl_multi_poll(multi, NULL, 0, 1000, NULL);

				if (mresult)
					break;
			}

			while ((msg = curl_multi_info_read(multi, &msgs_left)) != NULL) {
				if (msg->msg == CURLMSG_DONE) {
					int idx;

					/* Find out which handle this message is about */
					for (idx = 0; idx < curls.size(); idx++) {
						int found = (msg->easy_handle == curls[idx]);
						if (found)
							break;
					}

					switch (idx) {
					case 0://HTTP_HANDLE
						logger.add_log("HTTP transfer completed with status: " + msg->data.result);
						break;
					case 1://FTP_HANDLE
						logger.add_log("FTP transfer completed with status : " + msg->data.result);
						break;
					}
				}
			}
			for (int i = 0; i < curls.size(); i++) {
				curl_multi_remove_handle(multi, curls[i]);
			}

			curl_multi_cleanup(multi);
		}

		curl_global_cleanup();
	}

}