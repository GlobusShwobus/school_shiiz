#pragma once

#include <string>
#include <string_view>

namespace badSQL
{
	namespace my_urls {

		static constexpr std::string_view steam_player_summary_baseurl =
			"https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/";

		static constexpr std::string_view steam_player_level_baseurl =
			"http://api.steampowered.com/IPlayerService/GetSteamLevel/v1/";

		static constexpr std::string_view steam_app_schema_baseurl =
			"http://api.steampowered.com/ISteamUserStats/GetSchemaForGame/v2/";

		static constexpr std::string_view steam_app_achievement_list_baseurl =
			"https://api.steampowered.com/ISteamUserStats/GetGlobalAchievementPercentagesForApp/v0002/";

		static constexpr std::string_view steap_app_achievement_list_player_baseurl =
			"http://api.steampowered.com/ISteamUserStats/GetPlayerAchievements/v1/";

		static constexpr std::string_view steam_player_games_list_baseurl =
			"https://api.steampowered.com/IPlayerService/GetOwnedGames/v0001/";

		static constexpr std::string_view steam_player_recent_activity_baseurl =
			"https://api.steampowered.com/IPlayerService/GetRecentlyPlayedGames/v0001/";
	}

	std::string makeurl_steam_app_schema                    (std::string_view api_key, std::string_view appid);
	std::string makeurl_steam_app_achievement_list          (std::string_view appid);
	std::string makeurl_steam_app_achievement_list_player   (std::string_view api_key, std::string_view steamid, std::string_view appid);
	std::string makeurl_steam_player_summary                (std::string_view api_key, std::string_view steamid);
	std::string makeurl_steam_player_level                  (std::string_view api_key, std::string_view steamid);
	std::string makeurl_steam_player_owned_games_list       (std::string_view api_key, std::string_view steamid);
	std::string makeurl_steam_player_activity               (std::string_view api_key, std::string_view steamid);

	//may or may not need these
	//static constexpr std::string_view urlT_userSteamLevel = "http://api.steampowered.com/IPlayerService/GetSteamLevel/v1/?key=&steamid=";//use this for steam level
	static constexpr std::string_view urlT_userSteamBadges = "http://api.steampowered.com/IPlayerService/GetBadges/v1/?key=&steamid=";//badge info
}