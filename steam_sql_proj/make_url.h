#pragma once

#include <format>
#include <string>
#include <string_view>

namespace badSQL {

	static constexpr std::string_view achievement_schema_base =
		"http://api.steampowered.com/ISteamUserStats/GetSchemaForGame/v2/";

	static constexpr std::string_view achievement_globalinfo_base =
		"https://api.steampowered.com/ISteamUserStats/GetGlobalAchievementPercentagesForApp/v0002/";

	static constexpr std::string_view achievement_playerinfo_base =
		"http://api.steampowered.com/ISteamUserStats/GetPlayerAchievements/v1/";

	static constexpr std::string_view player_summary_base =
		"https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/";

	static constexpr std::string_view player_owned_games_base =
		"https://api.steampowered.com/IPlayerService/GetOwnedGames/v0001/";

	static constexpr std::string_view player_recent_activity_base =
		"https://api.steampowered.com/IPlayerService/GetRecentlyPlayedGames/v0001/";


	std::string make_achievement_schema_url(std::string_view key, std::string_view appid)
	{
		return std::format(
			"{}?key={}?appid={}&l=en",
			achievement_schema_base,
			key,
			appid
		);
	}

	std::string make_achievement_globalinfo_url(std::string_view appid)
	{
		return std::format("{}?gameid={}&format=json",
			achievement_globalinfo_base,
			appid);
	}

	std::string make_achievement_playerinfo_url(std::string_view key, std::string_view steamid, std::string_view appid)
	{
		return std::format(
			"{}?key={}&steamid={}&appid={}&l=en",
			achievement_playerinfo_base,
			key,
			steamid,
			appid
		);
	}

	std::string make_player_summary_url(std::string_view key, std::string_view steamid)
	{
		return std::format(
			"{}?key={}&steamids={}&format=json",
			player_summary_base,
			key,
			steamid
		);
	}

	std::string make_player_owned_games_url(std::string_view key, std::string_view steamid)
	{
		return std::format(
			"{}?key={}&steamid={}&include_appinfo=true&include_played_free_games=true&format=json",
			player_owned_games_base,
			key,
			steamid
		);
	}

	std::string make_player_recent_activity_url(std::string_view key, std::string_view steamid)
	{
		return std::format(
		"{}?key={}&steamid={}&format=json",
			player_recent_activity_base,
			key,
			steamid
		);
	}


	static constexpr std::string_view urlT_userSteamLevel = "http://api.steampowered.com/IPlayerService/GetSteamLevel/v1/?key=&steamid=";//use this for steam level
	static constexpr std::string_view urlT_userSteamBadges = "http://api.steampowered.com/IPlayerService/GetBadges/v1/?key=&steamid=";//also steam level, but no use for other data in current scope, not bad tho
}