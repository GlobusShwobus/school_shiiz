#pragma once

#include <string>
#include <vector>
#include "Logger.h"

#include <curl/curl.h>

namespace badSQL 
{
	struct WebRequestHandle {
		std::vector<char> data;
		long httpcode = -1;
		bool is_good = false;
		std::string contentType;
	};


	std::size_t call_back(void* content, std::size_t size, std::size_t nmemb, void* clientp)
	{
		size_t realSize = size * nmemb;

		auto* handle = static_cast<WebRequestHandle*>(clientp);
		auto* bytes = static_cast<const char*>(content);

		handle->data.insert(
			handle->data.end(),
			bytes,
			bytes + realSize
		);
		return realSize;
	}

	// returns true on success, false on failure
	// on failure call Logger::last_error for more info
	bool validate_certificate(std::string path)
	{
		CURL* curl = curl_easy_init();

		if (!curl) {
			return false;
		}

		curl_easy_setopt(curl, CURLOPT_CAINFO, path.c_str());

		//test on local host
		curl_easy_setopt(curl, CURLOPT_URL, "https://localhost");
		curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);

		CURLcode response = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		return response == CURLE_OK || response == CURLE_COULDNT_CONNECT;
	}


	WebRequestHandle request_data(std::string url, std::string certificate)
	{
		WebRequestHandle handle;
		CURL* curl;
		CURLcode result;

		curl = curl_easy_init();

		if (curl == NULL) {
			//if (extraInfo)
			//	*extraInfo = "libcurl failed to initalize";
			return false;
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.data());//fuck my ass, insert url as string -> get wider asshole (good i commented this)
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &handle);

		curl_easy_setopt(curl, CURLOPT_CAINFO, certificate.c_str());

		result = curl_easy_perform(curl);

		if (result != CURLE_OK) {
			curl_easy_cleanup(curl);
			//if (extraInfo)
			//	*extraInfo = "Libcurl err: " + std::string(curl_easy_strerror(result));
			return false;
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, reciever.httpcode);
		char* content_type = nullptr;
		curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
		if (content_type)
			reciever.contentType = content_type;

		curl_easy_cleanup(curl);
		reciever.isSuccess = true;
		//if (extraInfo)
		//	*extraInfo = "success";

		return handle;
	}

}