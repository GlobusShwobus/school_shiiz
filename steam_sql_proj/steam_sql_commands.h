#pragma once

#include <string_view>
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
namespace badSQL
{
	//NOTE: requires an external check to avoid name clashes
	std::string CREATE_DATABASE(std::string_view name)
	{
		return "CREATE DATABASE " + std::string(name) + ";";
	}
	//NOTE: requires an external check
	std::string USE_DATABASE(std::string_view name)
	{
		return "USE " + std::string(name) + ";";
	}

	constexpr std::string_view create_table_player_summary() noexcept
	{
		return
			"CREATE TABLE summary ("
					"user_id BIGINT PRIMARY KEY, "
					"name VARCHAR(25), "
					"level INT, "
					"country_code VARCHAR(10), "
					"creation_date DATE, "
					"account_url VARCHAR(125)"
			");";
	}
	constexpr std::string_view insert_into_player_summary() noexcept
	{
		return 
			"INSERT INTO summary ("
					"user_id, "
					"name, "
					"level, "
					"country_code, "
					"creation_date, " 
					"account_url"
				") "
			"VALUES (?,?,?,?,?,?);";
	}

	constexpr std::string_view create_table_game_info() noexcept
	{
		return
			"CREATE TABLE games ("
					"game_id BIGINT PRIMARY KEY, " 
					"name VARCHAR(75), "
					"game_version INT, "
					"total_playtime INT, "
					"last_played_date DATE"
			");";
	}
	constexpr std::string_view insert_into_game_info() noexcept
	{
		return
			"INSERT INTO games ("
					"game_id, "
					"name, "
					"game_version, " 
					"total_playtime, "
					"last_played_date"
				") "
			"VALUES (?,?,?,?,?);";
	}

	constexpr std::string_view create_table_recent_activity() noexcept
	{ 
		return
			"CREATE TABLE activity ("
					"game_id BIGINT PRIMARY KEY, " 
					"name VARCHAR(75), "
					"total_playtime INT, " 
					"playtime_fortnite INT"
			");";
	}
	constexpr std::string_view insert_into_recent_activity() noexcept
	{
		return
			"INSERT INTO activity ("
					"game_id, "
					"name, "
					"total_playtime, "
					"playtime_fortnite"
				") "
			"VALUES (?,?,?,?);";
	}

	constexpr std::string_view create_table_achievement_info() noexcept
	{
		return
			"CREATE TABLE achievements ("
					"game_id BIGINT, "
					"achievement_id VARCHAR(40), "
					"name VARCHAR(100), "
					"description VARCHAR(256), " 
					"global_completion DECIMAL(5,2), "
					"is_achieved INTEGER, "
					"unlock_date DATE, "
				"PRIMARY KEY(game_id, achievement_id)"
			");";
	}
	constexpr std::string_view insert_into_achievement_info() noexcept
	{
		return
			"INSERT INTO achievements ("
					"game_id, "
					"achievement_id, "
					"name, " 
					"description, "
					"global_completion, " 
					"is_achieved, "
					"unlock_date"
				") " 
			"VALUES (?,?,?,?,?,?,?);";
	}
}