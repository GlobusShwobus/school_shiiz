#pragma once

#include "SQLTable.h"

namespace ORDO {

	class SteamSummary :public SQLTable {
	public:
		std::string name;
		std::string accountUrl;
		std::string countryCode;
		std::string creationDate;
		long long playerId = 0;
		int playerLevel = 0;

		std::string_view tableName()const override;
		std::string_view createTableStatement()const override;
		std::string_view createInsertStatement()const override;
		void bindToStatement(sql::PreparedStatement* stmt)const override;
	};

	class SteamGame : public SQLTable {
	public:
		std::string gameName;
		std::string lastPlayedDate;
		int gameVersion = 0;//not sure currently if i can extract this, it's located in the achievement metadata, which is kind of fucky
		int gameID = 0;
		int playTimeForever = 0;

		std::string_view tableName()const override;
		std::string_view createTableStatement()const override;
		std::string_view createInsertStatement()const override;
		void bindToStatement(sql::PreparedStatement* stmt)const override;
	};

	class SteamActivity :public SQLTable {
	public:
		std::string gameName;
		int gameID = 0;
		int playtimeLastTwoWeeks = 0;
		int playtimeForever = 0;

		std::string_view tableName()const override;
		std::string_view createTableStatement()const override;
		std::string_view createInsertStatement()const override;
		void bindToStatement(sql::PreparedStatement* stmt)const override;
	};


	class SteamAchievements : public SQLTable {
	public:
		std::string achievementID;
		std::string achievementName;
		std::string description;
		std::string unlockDate;
		int gameID = 0;
		double globalCompRate = 0;
		bool isAchieved = 0;

		std::string_view tableName()const override;
		std::string_view createTableStatement()const override;
		std::string_view createInsertStatement()const override;
		void bindToStatement(sql::PreparedStatement* stmt)const override;
	};
}
