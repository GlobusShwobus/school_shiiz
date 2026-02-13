#define _CRTDBG_MAP_ALLOC  

#include <string>
#include <iostream>
#include "NumberGenerator.h"
#include "Sequence.h"
#include <curl/curl.h>
#include "SQL_inserter.h"
#include "Stuff.h"
#include "Payload.h"

using namespace badSQL;



int main() {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

    //"INSERT INTO ray_payloads (steadid, payload_type, payload) VALUES(?,?,?);"
    {
        CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
        if (result != CURLE_OK) {
            std::cout << curl_easy_strerror(result) << '\n';
            return -1;
        }

          
        
        SQLInserter testing_fails;
        std::cout<<"\n\n"<<testing_fails.connect(host, ip);


        Payload load;
        load.recipient_id = 667;
        load.label = "summary";
        load.data = R"({"hello_world": 67})";

        sql_insert_statement stmt("steamdb", "raw_payloads");
        stmt.fields.push_back("steamid");
        stmt.fields.push_back("payload_type");
        stmt.fields.push_back("payload");

        std::string response = testing_fails.inject(load, stmt);
        
        std::cout << "\n#############\n" << response << "\n#############\n";
        
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