#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "NumberGenerator.h"
#include "web_request.h"
#include "make_url.h"
#include "sql_command.h"

#include "json.hpp"


#include "SQLFlight.h"




using namespace badSQL;
/*
SAVE LOICENSE PATH IN JSON (maybe other shit too, pobably not api key and sql psswords tho)
*/

/*
* possible to do a vanity check for steam API to check validity < ResolveVanityURL >
*/

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

    std::string steam_id;
    std::string api_key;
    std::string cert_path;

    std::string games_list_url = make_player_summary_url(api_key, steam_id);

   // bool is_good_cert = validate_certificate(cert_path);
    std::cout << "your url: " << games_list_url << '\n';
    //std::cout << "your cert status: " << is_good_cert << '\n';

    auto data = request_data(games_list_url, cert_path);



    nlohmann::json json = nlohmann::json::parse(data.data);

    std::cout << json.dump(4) << '\n';


    DBConnect sqlcon;
    
    std::cout << sqlcon.connect() << '\n';

    std::string do_db_command = create_database_command("bill_clinton");
    std::cout << sqlcon.do_simple_command(do_db_command);
    std::cout << sqlcon.do_simple_command(create_use_database_command("bill_clinton"));
    std::string create_table_command = create_table_player_summary();

    std::cout << sqlcon.do_simple_command(create_table_command);

    std::string insert_table_command = insert_into_player_summary();
    player_summary_info info(
    "albert","meme123","geb","2025-02-05",123123,69
    );
    std::cout << sqlcon.do_prepared_statement(insert_table_command, info, bind_player_summary_info);

}

int main() {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

    {
        test123();

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