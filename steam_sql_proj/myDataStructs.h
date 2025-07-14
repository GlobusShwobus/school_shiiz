#pragma once

#include "SQLSchema.h"

namespace ORDO {

	class SteamSummary :public SQLSchema {
	public:
		std::string name;
		std::string accountUrl;
		std::string countryCode;
		std::string creationDate;
		long long playerId = 0;
		int playerLevel = 0;

		sqlCommand             createTableCommand()const override;
		sqlInsertOp            insertQuery()const override;
		std::string_view       getTableName()const override;
	};

	class SteamGame : public SQLSchema {
	public:
		std::string gameName;
		std::string lastPlayedDate;
		int gameVersion = 0;//not sure currently if i can extract this, it's located in the achievement metadata, which is kind of fucky
		int gameID = 0;
		int playTimeForever = 0;

		sqlCommand             createTableCommand()const override;
		sqlInsertOp            insertQuery()const override;
		std::string_view       getTableName()const override;
	};

	class SteamActivity :public SQLSchema {
	public:
		std::string gameName;
		int gameID = 0;
		int playtimeLastTwoWeeks = 0;
		int playtimeForever = 0;

		sqlCommand             createTableCommand()const override;
		sqlInsertOp            insertQuery()const override;
		std::string_view       getTableName()const override;
	};


	class SteamAchievements : public SQLSchema {
	public:
		std::string achievementID;
		std::string achievementName;
		std::string description;
		std::string unlockDate;
		int gameID = 0;
		double globalCompRate = 0;
		bool isAchieved = 0;

		sqlCommand             createTableCommand()const override;
		sqlInsertOp            insertQuery()const override;
		std::string_view       getTableName()const override;
	};
}
