#include "myDataStructs.h"

namespace ORDO {

	std::string_view SteamSummary::tableName()const 
	{
		static constexpr std::string_view name = "summary";
		return name;
	}
	std::string_view SteamSummary::createTableStatement()const 
	{
		static constexpr std::string_view createTable =
			"CREATE TABLE summary (user_id BIGINT PRIMARY KEY, name VARCHAR(25), level INT, country_code VARCHAR(10), creation_date DATE, account_url VARCHAR(125)); ";
		return createTable;
	}
	std::string_view SteamSummary::createInsertStatement()const
	{
		static constexpr std::string_view insertable =
			"INSERT INTO summary (user_id, name, level, country_code, creation_date, account_url) VALUES (?,?,?,?,?,?);";
		return insertable;
	}
	void SteamSummary::bindToStatement(sql::PreparedStatement* stmt)const
	{
		if (!stmt)
			throw std::runtime_error("prepared statement == nullptr");
		stmt->setUInt64(1, playerId);
		stmt->setString(2, name);
		stmt->setInt(3, playerLevel);
		stmt->setString(4, countryCode);
		stmt->setDateTime(5, creationDate);
		stmt->setString(6, accountUrl);
	}

	std::string_view SteamGame::tableName()const
	{
		static constexpr std::string_view name = "games";
		return name;
	}
	std::string_view SteamGame::createTableStatement()const
	{
		static constexpr std::string_view createTable =
			"CREATE TABLE games (game_id BIGINT PRIMARY KEY, name VARCHAR(75), game_version INT, total_playtime INT, last_played_date DATE); ";
		return createTable;
	}
	std::string_view SteamGame::createInsertStatement()const
	{
		static constexpr std::string_view insertable =
			"INSERT INTO games (game_id, name, game_version, total_playtime, last_played_date) VALUES (?,?,?,?,?);";
		return insertable;
	}
	void SteamGame::bindToStatement(sql::PreparedStatement* stmt)const
	{
		if (!stmt)
			throw std::runtime_error("prepared statement == nullptr");
		stmt->setUInt64(1, gameID);
		stmt->setString(2, gameName);
		stmt->setInt(3, gameVersion);
		stmt->setInt(4, playTimeForever);
		stmt->setDateTime(5, lastPlayedDate);
	}

	std::string_view SteamActivity::tableName()const
	{
		static constexpr std::string_view name = "activity";
		return name;
	}
	std::string_view SteamActivity::createTableStatement()const
	{
		static constexpr std::string_view createTable =
			"CREATE TABLE activity (game_id BIGINT PRIMARY KEY, name VARCHAR(75), total_playtime INT, playtime_fortnite INT); ";
		return createTable;
	}
	std::string_view SteamActivity::createInsertStatement()const
	{
		static constexpr std::string_view insertable =
			"INSERT INTO activity (game_id, name, total_playtime, playtime_fortnite) VALUES (?,?,?,?);";
		return insertable;
	}
	void SteamActivity::bindToStatement(sql::PreparedStatement* stmt)const
	{
		if (!stmt)
			throw std::runtime_error("prepared statement == nullptr");
		stmt->setUInt64(1, gameID);
		stmt->setString(2, gameName);
		stmt->setInt(3, playtimeForever);
		stmt->setInt(4, playtimeLastTwoWeeks);
	}

	std::string_view SteamAchievements::tableName()const
	{
		static constexpr std::string_view name = "achievements";
		return name;
	}
	std::string_view SteamAchievements::createTableStatement()const
	{
		static constexpr std::string_view createTable =
			"CREATE TABLE achievements (game_id BIGINT, achievement_id VARCHAR(40), name VARCHAR(100), description VARCHAR(256), global_completion DECIMAL(5,2), is_achieved INTEGER, unlock_date DATE, PRIMARY KEY(game_id, achievement_id)); ";
		return createTable;
	}
	std::string_view SteamAchievements::createInsertStatement()const
	{
		static constexpr std::string_view insertable =
			"INSERT INTO achievements (game_id, achievement_id, name, description, global_completion, is_achieved, unlock_date ) VALUES (?,?,?,?,?,?,?);";
		return insertable;
	}
	void SteamAchievements::bindToStatement(sql::PreparedStatement* stmt)const
	{
		if (!stmt)
			throw std::runtime_error("prepared statement == nullptr");
		stmt->setUInt64(1, gameID);
		stmt->setString(2, achievementID);
		stmt->setString(3, achievementName);
		stmt->setString(4, description);
		stmt->setDouble(5, globalCompRate);
		stmt->setBoolean(6, isAchieved);
		stmt->setDateTime(7, unlockDate);
	}
}