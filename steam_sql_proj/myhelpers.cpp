#include "myhelpers.h"
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <format>
namespace ORDO {
	namespace lazy {
        void RemoveSymbols(std::string& from, const std::string& symbols) {
            from.erase(std::remove_if(from.begin(), from.end(), [&symbols](const char c) {
                return symbols.find(c) != std::string::npos;
                }), from.end());
        }
        void RemoveSymbols(std::string& from, const char symbol) {
            from.erase(std::remove_if(from.begin(), from.end(), [symbol](const char c) {
                return symbol == c;
                }), from.end());
        }
        void PrintList(const std::vector<std::pair<std::string, std::string>>& list) {
            size_t i = 1;
            for (const auto& each : list) {
                std::cout << std::setw(5) << i++ << " " << each.second << "\n";
            }
        }
        bool ListContains(const std::vector<std::string>& list, const std::string& lookFor) {
            return std::find(list.begin(), list.end(), lookFor) != list.end();
        }
        const std::string UnixTime(time_t unix_timestamp) {
            std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(unix_timestamp);
            return std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(tp));
        }
        std::string GetClipboard() {
            if (!OpenClipboard(nullptr)) {
                return "";
            }
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData == nullptr) {
                CloseClipboard();
                return "";
            }

            char* copy_stream = static_cast<char*>(GlobalLock(hData));//do not delete OR MEMES
            std::string text;

            if (copy_stream) {
                text = copy_stream;
            }

            GlobalUnlock(hData);
            CloseClipboard();

            return text;
        }



        void console_wait() {
            if (std::cin.rdbuf()->in_avail() > 0)
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
        void console_title() {
            static constexpr std::string_view title =
                R"(/'\_/`\ _
|     |(_)  ___  _   _   ___ ___     __    ___     _ _
| (_) || |/',__)( ) ( )/' _ ` _ `\ /'__`\/' _ `\ /'_` )
| | | || |\__, \| (_) || ( ) ( ) |(  ___/| ( ) |( (_| |
(_) (_)(_)(____/`\___/'(_) (_) (_)`\____)(_) (_)`\__,_)                                                  
                   )";
            static constexpr std::string_view padding =
                "#######################################################################";
            std::cout << title << '\n' << padding << '\n';
        }
        void console_clear() {
            system("cls");
        }


	}
}