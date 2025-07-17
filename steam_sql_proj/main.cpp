#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  


#include"web_request.h"
#include "json.hpp"
#include "Inputs.h"
#include "myHelpers.h"
#include "steamURlEditor.h"
#include "SQLUser.h"
#include "SQLFlight.h"
#include "myDataStructs.h"
#include "NumberGenerator.h"


using namespace ORDO;
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
 
 	NumberGenerator rng;
 
 	for (int i = 0; i < 10; i++)
 	{
 		int dice = rng.getRandInt(0, 2);
 		char c;
 		switch (dice) {
 		case 0:c = rng.getRandInt(ASCII_NUM_START      , ASCII_NUM_END);	   break;
 		case 1:c = rng.getRandInt(ASCII_BIGCHAR_START  , ASCII_BIGCHAR_END);   break;
 		case 2:c = rng.getRandInt(ASCII_SMALLCHAR_START, ASCII_SMALLCHAR_END); break;
 		default:
 			printf("random name generation RNG missfire\n");
 			continue;
 		}
 		prefix.push_back(c);
 	}
 	return prefix;
 }
 //#########################################
void testSQLFlightSystem() {

    const std::string schemaName = GenerateRandomName("LANDING_");  //important diff, need to assign some name before, otherwise use setSchemaName

    SQLUser user;
    SteamSummary summary(schemaName);
    SQLFlight flight;

    lazy::console_title();

    //initialize summary members
    summary.name = "Bob";
    summary.accountUrl = "zombocom";
    summary.countryCode = "EE";
    long long lalalong = 946684800;//jan 1st 2000
    summary.creationDate = lazy::UnixTime(lalalong);
    summary.playerId = 123456;
    summary.playerLevel = 69;
    //##############################

    //ask for IP, sql user, and sql password
    std::string IP;
    std::string username;
    std::string password;
    std::cout << "Enter IP: ";
    IP = Inputs::InputStr();
    std::cout << "Enter username: ";
    username = Inputs::InputStr();
    std::cout << "Enter password: ";
    password = Inputs::InputStr();

    user = SQLUser(IP, username, password);
    
    flight.connect(user.getIP(), user.getSName(), user.getSPassword());
    if (flight.isGood()) {

        flight.doCommand(summary.createSchemaCommand());
        flight.setSchema(summary.getSchemaName());
        flight.doCommand(summary.createTableCommand());
        flight.doPreparedInsert(summary.insertQuery());
        std::cout << '\n' << "SUCCESS";
        lazy::console_wait();
    }
    else {
        std::cout << '\n' << flight.getStreamError();
        lazy::console_wait();
        return;
    }
}

int main() {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

    testSQLFlightSystem();

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