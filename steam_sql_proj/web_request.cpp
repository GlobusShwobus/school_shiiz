#include "web_request.h"
#include <fstream>
#include <curl/curl.h>

namespace badSQL {
	size_t Request::callBack(void* contents, size_t size, size_t nmemb, void* clientp) {
		size_t realSize = size * nmemb;
		ResoponseBuffer* buffer = static_cast<ResoponseBuffer*>(clientp);
		char* begin = static_cast<char*>(contents);
		buffer->data.insert(buffer->data.end(), begin, begin + realSize);
		return realSize;
	}
	bool Request::requestProtocol(std::string_view url, ResoponseBuffer& reciever, std::string* extraInfo) {
		CURL* curl;
		CURLcode result;

		curl = curl_easy_init();

		if (curl == NULL) {
			if (extraInfo)
				*extraInfo = "libcurl failed to initalize";
			return false;
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.data());//fuck my ass, insert url as string -> get wider asshole (good i commented this)
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Request::callBack);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reciever);

		curl_easy_setopt(curl, CURLOPT_CAINFO, loicense.c_str());

		result = curl_easy_perform(curl);

		if (result != CURLE_OK) {
			curl_easy_cleanup(curl);
			if (extraInfo)
				*extraInfo = "Libcurl err: " + std::string(curl_easy_strerror(result));
			return false;
		}
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, reciever.httpcode);
		char* content_type = nullptr;
		curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
		if (content_type)
			reciever.contentType = content_type;

		curl_easy_cleanup(curl);
		reciever.isSuccess = true;
		if (extraInfo)
			*extraInfo = "success";
		return true;
	}

	std::unique_ptr<Request::ResoponseBuffer> Request::request(std::string_view url, std::string* extraInfo)
	{
		if (loicense.empty()) {
			if (extraInfo)
				*extraInfo = "Path to certificate cacert.pem not established";
			return nullptr;
		}
		std::unique_ptr<ResoponseBuffer> reciever = std::make_unique<ResoponseBuffer>();

		if (!requestProtocol(url, *reciever, extraInfo))
			reciever.get()->data.clear();

		//func() http code error message not here actually
		return std::move(reciever);
	}
}