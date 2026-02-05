#pragma once
#include <string_view>
#include <cppconn/prepared_statement.h>
#include "Sequence.h"

namespace badSQL
{
	struct player_summary_info
	{
		std::string name;
		std::string accountUrl;
		std::string countryCode;
		std::string creationDate;
		long long playerId = 0;
		int playerLevel = 0;
	};

	struct game_info
	{
		std::string gameName;
		std::string lastPlayedDate;
		int gameVersion = 0;//not sure currently if i can extract this, it's located in the achievement metadata, which is kind of fucky
		int gameID = 0;
		int playTimeForever = 0;
	};

	struct recent_activity_info
	{
		std::string gameName;
		int gameID = 0;
		int playtimeLastTwoWeeks = 0;
		int playtimeForever = 0;
	};

	struct achievement_info
	{
		std::string achievementID;
		std::string achievementName;
		std::string description;
		std::string unlockDate;
		int gameID = 0;
		double globalCompRate = 0;
		bool isAchieved = 0;
	};



	std::string create_database_command(std::string_view name) {
		return "CREATE DATABASE " + std::string(name) + ";";
	}
	std::string create_use_database_command(std::string_view name) {
		return "USE " + std::string(name) + ";";
	}

	std::string create_table_player_summary()
	{
		return
			"CREATE TABLE summary (user_id BIGINT PRIMARY KEY, name VARCHAR(25), level INT, country_code VARCHAR(10), creation_date DATE, account_url VARCHAR(125)); ";
	}
	std::string insert_into_player_summary()
	{
		return 
			"INSERT INTO summary (user_id, name, level, country_code, creation_date, account_url) VALUES (?,?,?,?,?,?);";
	}
	void bind_player_summary_info(sql::PreparedStatement* pstmt, const player_summary_info& info)
	{
		pstmt->setUInt64(1, info.playerId);
		pstmt->setString(2, info.name);
		pstmt->setInt(3, info.playerLevel);
		pstmt->setString(4, info.countryCode);
		pstmt->setDateTime(5, info.creationDate);
		pstmt->setString(6, info.accountUrl);
	}


	std::string create_table_game_info()
	{
		return
			"CREATE TABLE games (game_id BIGINT PRIMARY KEY, name VARCHAR(75), game_version INT, total_playtime INT, last_played_date DATE); ";
	}
	std::string insert_into_game_info() 
	{
		return
			"INSERT INTO games (game_id, name, game_version, total_playtime, last_played_date) VALUES (?,?,?,?,?);";
	}
	void bind_game_info(sql::PreparedStatement* pstmt, const game_info& info)
	{
		pstmt->setUInt64(1, info.gameID);
		pstmt->setString(2, info.gameName);
		pstmt->setInt(3, info.gameVersion);
		pstmt->setInt(4, info.playTimeForever);
		pstmt->setDateTime(5, info.lastPlayedDate);
	}


	std::string create_table_recent_activity() 
	{
		return
			"CREATE TABLE activity (game_id BIGINT PRIMARY KEY, name VARCHAR(75), total_playtime INT, playtime_fortnite INT); ";
	}
	std::string insert_into_recent_activity()
	{
		return
			"INSERT INTO activity (game_id, name, total_playtime, playtime_fortnite) VALUES (?,?,?,?);";
	}
	void bind_recent_activity(sql::PreparedStatement* pstmt, const recent_activity_info& info)
	{
		pstmt->setUInt64(1, info.gameID);
		pstmt->setString(2, info.gameName);
		pstmt->setInt(3, info.playtimeForever);
		pstmt->setInt(4, info.playtimeLastTwoWeeks);
	}



	std::string create_table_achievement_info()
	{
		return
			"CREATE TABLE achievements (game_id BIGINT, achievement_id VARCHAR(40), name VARCHAR(100), description VARCHAR(256), global_completion DECIMAL(5,2), is_achieved INTEGER, unlock_date DATE, PRIMARY KEY(game_id, achievement_id)); ";
	}
	std::string insert_into_achievement_info()
	{
		return
			"INSERT INTO achievements (game_id, achievement_id, name, description, global_completion, is_achieved, unlock_date ) VALUES (?,?,?,?,?,?,?);";
	}
	void bind_achievement_info(sql::PreparedStatement* pstmt, const achievement_info& info)
	{
		pstmt->setUInt64(1, info.gameID);
		pstmt->setString(2, info.achievementID);
		pstmt->setString(3, info.achievementName);
		pstmt->setString(4, info.description);
		pstmt->setDouble(5, info.globalCompRate);
		pstmt->setBoolean(6, info.isAchieved);
		pstmt->setDateTime(7, info.unlockDate);
	}


	/// TODO::
	/// CUSTOM SECTION
	/// 
	/// 
	struct Column
	{
		std::string name;
		std::string type; //SQL types
		Sequence<std::string> constraints;// NOT NULL, AUTO_INCREMENT... should not be sued for primary and foreign keys
	};

	// EXAMPLE USAGE:
	//                  auto sql = create_table_command(
	//                  	"achievements",                                            
	//                  	{
	//                  		{ "game_id", "BIGINT", { "NOT NULL" } },
	//                  		{ "achievement_id", "VARCHAR(40)", { "NOT NULL" } },
	//                  		{ "name", "VARCHAR(100)" },
	//                  		{ "description", "VARCHAR(256)" }
	//                  	},
	//                  {
	//                  	"PRIMARY KEY (game_id, achievement_id)"
	//                  }
	//                  );
	std::string create_custom_table_command(std::string_view table_name, Sequence<Column> columns, Sequence<std::string> table_constraints = {})
	{
		std::string command = "CREATE TABLE ";
		command += table_name;
		command += " (";

		bool first = true;
		for (const auto& col : columns) {

			command += col.name + ' ' + col.type;	       // add the name and the type of the field
													       //
			                                               // 
			for (const auto& c : col.constraints) {        // add constraints associated with the given type
 				command += ' ' + c;					       
			}										       
													       
			if (!first) 							       // add a comma after every field except the first iteration
				command += ", ";					       //
			first = false;							       // ...might get optimzed away
		}

		for (const auto& constraint : table_constraints) { //add table level constrains, mainly key info
			command += ", " + constraint;
		}

		command += ");";

		return command;
	}



	//TODO:: function to see if custom table and insert commands fit

}