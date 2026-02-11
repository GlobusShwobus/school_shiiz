#pragma once

#include <string>
#include <cppconn/prepared_statement.h>
#include <optional>
#include <charconv>
#include "json.hpp"
#include "Stuff.h"
#include "Sequence.h"
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
// DEPRICATED
//TODO:: extract data from json. probably make class types instead.
namespace badSQL
{
	struct extract_result
	{
		bool valid = false;    //for is bindable
		std::string message;   //some message
	};

	//##############################################################################################################
	struct POD_player_summary
	{
		long long steamid;
		std::optional<std::string> personaname;
		std::optional<std::string> locountrycode;
		std::optional<std::string> timecreated;
		std::optional<std::string> profileurl;
	};
	extract_result extractjson_player_summary(const nlohmann::json& info, POD_player_summary& data_sheet)
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

		const auto& data = (*players).front();// players is an array, so grab the first object.. why is it an array???

		//steamid is a required field due to being a primary key
		const auto id = data.find("steamid");
		if (id == data.end() || id->empty() || !id->is_string()) {
			result.message = "Missing steamid";
			return result;
		}

		const auto& id_str = id->get<std::string>();
		long long steamid = 0;
		//c++17 way to check a sequence of characters to match actual ints (or floats for the other one)
		//basically checks all chars. ptr will point to the first char that can't be inserted into steamid (or end if success)
		//ec is error code type
		//it will always read into steamid valid chars, so a error code check MUST be done
		auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), steamid);//why ptr instead of iterator...? whatever

		if (ec == std::errc{} || ptr == id_str.data() + id_str.size()) {
			result.message = "Malformed steamid";
			return result;
		}
		data_sheet.steamid = steamid;


		//optional fields
		const bool has_personaname   = data.contains("personaname");
		const bool has_locountrycode = data.contains("loccountrycode");
		const bool has_profileurl    = data.contains("profileurl");
		const bool has_timecreated   = data.contains("timecreated");

		if (has_personaname && !data["personaname"].empty() && data["personaname"].is_string())
			data_sheet.personaname = data["personaname"].get<std::string>();
		
		if (has_locountrycode && !data["loccountrycode"].empty() && data["loccountrycode"].is_string())
			data_sheet.locountrycode = data["loccountrycode"].get<std::string>();
		
		if (has_profileurl && !data["profileurl"].empty() && data["profileurl"].is_string())
			data_sheet.profileurl = data["profileurl"].get<std::string>();

		if (has_timecreated && !data["timecreated"].empty() && data["timecreated"].is_number()) {
			long long time = data["timecreated"].get<long long>();
			data_sheet.timecreated = ll_to_unix_time(time);
		}

		result.valid = true;
		result.message = "Player summary info extracted";

		return result;
	}
	//##############################################################################################################
	struct POD_player_level
	{
		std::optional<int> player_level;
	};
	extract_result extractjson_player_level(const nlohmann::json& info, POD_player_level& data_sheet)
	{
		extract_result result;
		if (!info.contains("response")) {
			result.message = "Missing 'response'";
			return result;
		}

		const auto& data = info["response"];

		if (!data.contains("player_level") || data["player_level"].empty() || !data["player_level"].is_number_integer()) {
			result.message = "Missing or invalid player_level";
			return result;
		}

		data_sheet.player_level = data["player_level"].get<int>();
		result.valid = true;
		result.message = "Player level info extracted";

		return result;
	}

	//##############################################################################################################
	struct POD_gameinfo {
		long long appid;
		std::optional<std::string> name;
		std::optional<int> playtime_forever;
		std::optional<std::string> rtime_last_played;
	};
	struct POD_player_owned_games
	{
		std::optional<int> game_count;
		Sequence<POD_gameinfo> games;
	};

	extract_result extractjson_player_owned_games(const nlohmann::json& info, POD_player_owned_games& data_sheet)
	{
		extract_result result;

		const auto response = info.find("response");
		if (response == info.end()) {
			result.message = "Missing 'response'";
			return result;
		}

		const auto games = response->find("games");
		if (games == response->end() || !games->is_array() || games->empty()) {
			result.message = "Missing or no 'games'";
			return result;
		}

		// handle the one extra field for games count
		const auto game_count = response->find("game_count");
		if (game_count != response->end() && !game_count->empty() && game_count->is_number())
			data_sheet.game_count = game_count->get<int>();

		// loop over all entries
		for (const auto& entry : *games) {
			POD_gameinfo gi;

			const bool has_appid = entry.contains("appid"); //appid is primary key, mandatory field

			if (!has_appid || entry["appid"].empty() || !entry["appid"].is_number()) { //technically dumb as fuck but at this point you never know. i don't trust steam anymore
				continue;
			}

			gi.appid = entry["appid"].get<long long>();

			const bool has_name = entry.contains("name");
			const bool has_playtime_forever = entry.contains("playtime_forever");
			const bool has_rtime_last_played = entry.contains("rtime_last_played");

			if (has_name && !entry["name"].empty() && entry["name"].is_string())
				gi.name = entry["name"].get<std::string>();

			if (has_playtime_forever && !entry["playtime_forever"].empty() && entry["playtime_forever"].is_number())
				gi.playtime_forever = entry["playtime_forever"].get<int>();

			if (has_rtime_last_played && !entry["rtime_last_played"].empty() && entry["rtime_last_played"].is_number()) {
				long long time = entry["rtime_last_played"].get<long long>();
				gi.rtime_last_played = ll_to_unix_time(time);
			}

			data_sheet.games.push_back(std::move(gi));
		}

		result.valid = true;
		result.message = "Player owned games info extracted";

		return result;
	}
	//##############################################################################################################
	struct POD_achievement {
		std::string apiname;
		std::string name;
		std::string unlocktime;
		std::string description;
		bool achieved;
	};
	struct POD_achievements {
		long long steamid;
		std::string gamename;
		Sequence<POD_achievement> achievements;
	};

	extract_result extractjson_player_achievements(const nlohmann::json& info, POD_achievements& data_sheet)
	{
		extract_result result;

		const auto playerstats = info.find("playerstats");
		if (playerstats == info.end()) {
			result.message = "Missing 'playerstats'";
			return result;
		}

		const auto achievements = playerstats->find("achievements");
		if (achievements == playerstats->end() || !achievements->is_array() || achievements->empty()) {
			result.message = "Missing or no 'achievements'";
			return result;
		}

		const auto steamID = playerstats->find("steamID");
		if (steamID != playerstats->end() && !steamID->empty() && steamID->is_string()){
			const auto& id_str = steamID->get<std::string>();
			long long steamid = 0;

			auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), steamid);

			if (ec == std::errc{} || ptr == id_str.data() + id_str.size()) {
				result.message = "Missing, malformed or doesn't own the app for steamid";//break because the data can't be associated with anything
				return result;
			}
			data_sheet.steamid = steamid;
		}

		const auto gameName = playerstats->find("gameName");
		if (gameName != playerstats->end() && !gameName->empty() && gameName->is_string())
			data_sheet.gamename = gameName->get<std::string>();

		for (const auto& entry : *achievements) {
			POD_achievement achv;

			if (entry.contains("apiname") && !entry["apiname"].empty() && entry["apiname"].is_string())
				achv.apiname = entry["apiname"].get<std::string>();

			if (entry.contains("achieved") && !entry["achieved"].empty() && entry["achieved"].is_boolean())
				achv.achieved = entry["achieved"].get<bool>();

			if (entry.contains("unlocktime") && !entry["unlocktime"].empty() && entry["unlocktime"].is_number()) {
				long long time = entry["unlocktime"].get<long long>();
				achv.unlocktime = ll_to_unix_time(time);
			}

			if (entry.contains("name") && !entry["name"].empty() && entry["name"].is_string())
				achv.name = entry["name"].get<std::string>();

			if (entry.contains("description") && !entry["description"].empty() && entry["description"].is_string())
				achv.description = entry["description"].get<std::string>();

			data_sheet.achievements.push_back(std::move(achv));
		}

		result.valid = true;
		result.message = "Player achievements per appid extracted";
		return result;
	}



	//##############################################################################################################

	struct POD_percentage {
		std::string apiname;
		float percent;
	};

	struct POD_percentage_list
	{
		Sequence<POD_percentage> list;
	};






	/*
		//void bind_player_summary_info(sql::PreparedStatement* pstmt, const player_summary_info& info)
	//{
	//	
	//	pstmt->setUInt64(1, info.steamid);
	//	pstmt->setString(2, info.personaname);
	//	pstmt->setInt(3, info.player_level);
	//	pstmt->setString(4, info.locountrycode);
	//	pstmt->setDateTime(5, info.timecreated);
	//	pstmt->setString(6, info.profileurl);
	//	
	//}
	// 	//void bind_game_info(sql::PreparedStatement* pstmt, const game_info& info)
	//{
	//	pstmt->setUInt64(1, info.appid);
	//	pstmt->setString(2, info.name);
	//	pstmt->setInt(3, info.gameVersion);
	//	pstmt->setInt(4, info.playtime_forever);
	//	pstmt->setDateTime(5, info.rtime_last_played);
	//}
		void bind_recent_activity(sql::PreparedStatement* pstmt, const recent_activity_info& info)
	{
		pstmt->setUInt64(1, info.gameID);
		pstmt->setString(2, info.gameName);
		pstmt->setInt(3, info.playtimeForever);
		pstmt->setInt(4, info.playtimeLastTwoWeeks);
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
	*/

}