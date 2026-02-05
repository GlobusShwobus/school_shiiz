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

}