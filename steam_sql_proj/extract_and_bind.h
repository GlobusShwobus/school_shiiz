#pragma once

#include <string>
#include <cppconn/prepared_statement.h>
#include "json.hpp"
#include <charconv>
#include "Stuff.h"

//TODO:: extract data from json. probably make class types instead.
namespace badSQL
{
	/*
				"CREATE TABLE summary ("
					"user_id BIGINT PRIMARY KEY, "
					"name VARCHAR(25), "
					"level INT, "
					"country_code VARCHAR(10), "
					"creation_date DATE, "
					"account_url VARCHAR(125)"
			");";
	*/

	struct extract_result
	{
		bool valid = false;    //for is bindable
		bool partial = false;  //for missing optional fields
		std::string message;   //some message
	};

	struct player_summary_info
	{
		long long steamid;
		std::string personaname;
		int player_level;
		std::string locountrycode;
		std::string timecreated;
		std::string profileurl;
	};

	extract_result extract_player_summary_json(const nlohmann::json& info, player_summary_info& data_sheet)
	{
		extract_result result;
		//top level json response check
		const auto response = info.find("response");
		if (response == info.end()) {
			result.message = "Missing 'response'";
			return result;
		}

		const auto players = response->find("players");
		if (players == response->end() || !players->is_array() || players->empty()) {
			result.message = "Missing player dataset";
			return result;
		}

		const auto& data = (*players)[0];

		//steamid is a required field due to being a primary key
		const auto id = data.find("steamid");
		if (id != data.end() && id->is_string()) {
			const auto& id_str = id->get<std::string>();
			long long steamid = 0;
			//c++17 way to check a sequence of characters to match actual ints (or floats for the other one)
			//basically checks all chars. ptr will point to the first char that can't be inserted into steamid (or end if success)
			//ec is error code type
			//it will always read into steamid valid chars, so a error code check MUST be done
			auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data()+id_str.size(), steamid);//why ptr instead of iterator...? whatever

			if (ec == std::errc{} || ptr == id_str.data() + id_str.size()) {
				result.message = "Missing or malformed steamid";
				return result;
			}
			data_sheet.steamid = steamid;
		}

		//optional fields
		const bool has_personaname   = data.contains("personaname");
		const bool has_locountrycode = data.contains("loccountrycode");
		const bool has_profileurl    = data.contains("profileurl");
		const bool has_timecreated   = data.contains("timecreated");

		if (has_personaname && data["personaname"].is_string()) {
			data_sheet.personaname = data["personaname"].get<std::string>();
		}

		if (has_locountrycode && data["loccountrycode"].is_string()) {
			data_sheet.locountrycode = data["loccountrycode"].get<std::string>();
		}

		if (has_profileurl && data["profileurl"].is_string()) {
			data_sheet.profileurl = data["profileurl"].get<std::string>();
		}

		if (has_timecreated && data["timecreated"].is_number()) {
			long long time = data["timecreated"].get<long long>();
			data_sheet.timecreated = ll_to_unix_time(time);
		}

		result.partial = !(
			has_personaname &&
			has_locountrycode &&
			has_profileurl &&
			has_timecreated
			);

		result.valid = true;

		result.message = result.partial
			? "Player summary extracted (partial)"
			: "Player summary extracted";

		return result;
	}
	extract_result extract_player_level_json(const nlohmann::json& info, player_summary_info& data_sheet)
	{
		extract_result result;
		if (!info.contains("response")) {
			
			result.message = "Missing 'response'";
			return result;
		}

		const auto& data = info["response"];

		if (!data.contains("player_level") || !data["player_level"].is_number_integer()) {
			result.message = "Missing or invalid player_level";
			return result;
		}

		data_sheet.player_level = data["player_level"].get<int>();
		result.valid = true;
		result.message = "Player level extracted";

		return result;
	}

	void bind_player_summary_info(sql::PreparedStatement* pstmt, const player_summary_info& info)
	{
		pstmt->setUInt64(1, info.steamid);
		pstmt->setString(2, info.personaname);
		pstmt->setInt(3, info.player_level);
		pstmt->setString(4, info.locountrycode);
		pstmt->setDateTime(5, info.timecreated);
		pstmt->setString(6, info.profileurl);
	}















	struct game_info
	{
		std::string gameName;
		std::string lastPlayedDate;
		int gameVersion = 0;//not sure currently if i can extract this, it's located in the achievement metadata, which is kind of fucky
		int gameID = 0;
		int playTimeForever = 0;
	};

	void bind_game_info(sql::PreparedStatement* pstmt, const game_info& info)
	{
		pstmt->setUInt64(1, info.gameID);
		pstmt->setString(2, info.gameName);
		pstmt->setInt(3, info.gameVersion);
		pstmt->setInt(4, info.playTimeForever);
		pstmt->setDateTime(5, info.lastPlayedDate);
	}

	struct recent_activity_info
	{
		std::string gameName;
		int gameID = 0;
		int playtimeLastTwoWeeks = 0;
		int playtimeForever = 0;
	};

	void bind_recent_activity(sql::PreparedStatement* pstmt, const recent_activity_info& info)
	{
		pstmt->setUInt64(1, info.gameID);
		pstmt->setString(2, info.gameName);
		pstmt->setInt(3, info.playtimeForever);
		pstmt->setInt(4, info.playtimeLastTwoWeeks);
	}

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

}