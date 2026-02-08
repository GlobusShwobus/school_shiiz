#pragma once
#include <string>
#include <cppconn/prepared_statement.h>

//TODO:: extract data from json. probably make class types instead.
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

	void bind_player_summary_info(sql::PreparedStatement* pstmt, const player_summary_info& info)
	{
		pstmt->setUInt64(1, info.playerId);
		pstmt->setString(2, info.name);
		pstmt->setInt(3, info.playerLevel);
		pstmt->setString(4, info.countryCode);
		pstmt->setDateTime(5, info.creationDate);
		pstmt->setString(6, info.accountUrl);
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