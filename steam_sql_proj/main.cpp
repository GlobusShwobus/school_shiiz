#define _CRTDBG_MAP_ALLOC  

#include <string>
#include <iostream>
#include "NumberGenerator.h"
#include "Sequence.h"
#include "steam_urls.h"
#include "web_request.h"
#include "json.hpp"

#include "Stuff.h"

using namespace badSQL;

//TODO:: create database name must be unique- make a checker function
//TODO:: use database may fail if db doesnt exist or typo- make a checker function
//TODO:: create table duplicate should fail safely

 std::string GenerateRandomName(std::string prefix = "") {
 	static constexpr int ASCII_NUM_START = 48;
 	static constexpr int ASCII_NUM_END = 57;
 	static constexpr int ASCII_BIGCHAR_START = 65;
 	static constexpr int ASCII_BIGCHAR_END = 90;
 	static constexpr int ASCII_SMALLCHAR_START = 97;
 	static constexpr int ASCII_SMALLCHAR_END = 122;
 
 	RandomNum rng;
 
 	for (int i = 0; i < 10; i++)
 	{
 		int dice = rng.rInt(0, 2);
 		char c;
 		switch (dice) {
 		case 0:c = rng.rInt(ASCII_NUM_START      , ASCII_NUM_END);	     break;
 		case 1:c = rng.rInt(ASCII_BIGCHAR_START  , ASCII_BIGCHAR_END);   break;
 		case 2:c = rng.rInt(ASCII_SMALLCHAR_START, ASCII_SMALLCHAR_END); break;
 		default:
 			printf("random name generation RNG missfire\n");
 			continue;
 		}
 		prefix.push_back(c);
 	}
 	return prefix;
 }
 //#########################################
void test123() {

    std::string cert_path="C:/Users/ADMIN/Desktop/tools/cacert.pem";
    //APP ID's
    std::vector<std::string> apps = {
           "1621690", "1091500", "374320", "1085660", "230230",
           "435150", "1888160", "33230", "48190"
    };

    Sequence<std::string> urls;
    for (const auto& app : apps) {
        urls.emplace_back(makeurl_steam_app_achievement_list(app));
        std::cout << "URL: " << urls.back() << '\n';
    }

    std::cout << "\n";

    auto results = multi_request_data(urls, cert_path);
    std::cout << "results size: " << results.size() << '\n';

    Sequence<nlohmann::json> jsons;
    int empty_responses = 0;
    int failed_responses = 0;
    int success_responses = 0;

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        std::cout << "App ID: " << apps[i] << "\n";
        std::cout << "HTTP Code: " << r.httpcode << "\n";
        std::cout << "Data size: " << r.data.size() << " bytes\n";

        if (r.is_good) {
            if (!r.data.isEmpty()) {
                try {
                    std::string response(r.data.data(), r.data.size());

                    // Check if it's empty JSON
                    if (response == "{}" || response.empty()) {
                        std::cout << "Status: No achievement data for this game\n";
                        empty_responses++;

                        // Still create empty JSON
                        jsons.emplace_back(nlohmann::json::object());
                    }
                    else {
                        nlohmann::json json = nlohmann::json::parse(r.data.data());
                        std::cout << "Status: Success (" << json.size() << " achievements)\n";
                        success_responses++;
                        jsons.push_back(std::move(json));
                    }
                }
                catch (const std::exception& e) {
                    std::cout << "Status: JSON Parse Error - " << e.what() << "\n";
                    failed_responses++;
                }
            }
            else {
                std::cout << "Status: Empty response (0 bytes)\n";
                empty_responses++;
                jsons.emplace_back(nlohmann::json::object());
            }
        }
        else {
            std::cout << "Status: FAIL (HTTP " << r.httpcode << ")\n";
            failed_responses++;

            // Print error data if available
            if (!r.data.isEmpty()) {
                std::string error(r.data.data(), core_min(r.data.size(), std::size_t(200)));
                std::cout << "Error response: " << error << "\n";
            }
        }
        std::cout << "---\n";
    }

    std::cout << "\nSummary:\n";
    std::cout << "Total requests: " << results.size() << "\n";
    std::cout << "Successful (with data): " << success_responses << "\n";
    std::cout << "Empty responses ({}): " << empty_responses << "\n";
    std::cout << "Failed responses: " << failed_responses << "\n\n";

    // Print non-empty JSONs
    std::cout << "Non-empty JSON data:\n";
    for (size_t i = 0; i < jsons.size(); ++i) {
        if (!jsons[i].empty()) {
            std::cout << "App " << apps[i] << ":\n";
            std::cout << jsons[i].dump(2) << "\n\n";
        }
    }
}

int main() {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);


    {
        CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
        if (result != CURLE_OK) {
            std::cout << curl_easy_strerror(result) << '\n';
            return -1;
        }

       // test123();

        std::cout << console_title() << '\n';

        curl_global_cleanup();
    }


    _CrtDumpMemoryLeaks();
    return 0;
}

/*
int main() {

    using namespace ORDO;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));


    std::unique_ptr<STEAM> handle = SteamLogIn();
    if (handle == nullptr) {
        printf("\nConnection proccess aborted. Can not continue without it.");
    }

    std::unique_ptr<DATABASE> database = SQLLogIn(*handle);
    if (database == nullptr) {
        printf("\nConnection proccess aborted. Can not continue without it.");
    }

    MDATA data_col;
    bool end_session = false;

    while (!end_session) {
        lazy::console_clear();
        lazy::console_title();

        const int select = Inputs::InputRange(1, 3, MAIN_PAGE);

        switch (select) {
        case 1:
            SteamPageMain(*handle, data_col);
            break;
        case 2:
            SQLPageMain(*database, data_col);
            break;
        case 3:
            end_session = true;
            break;
        }
    }

    return 0;
}
*/