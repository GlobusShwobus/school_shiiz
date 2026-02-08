#include "steam_urls.h"
#include <format>

namespace badSQL
{
	std::string makeurl_steam_app_schema(std::string_view api_key, std::string_view appid)
	{
		return std::format(
			"{}?key={}&appid={}&l=en",
			my_urls::steam_app_schema_baseurl,
			api_key,
			appid
		);
	}
	std::string makeurl_steam_app_achievement_list(std::string_view appid)
	{
		return std::format("{}?gameid={}&format=json",
			my_urls::steam_app_achievement_list_baseurl,
			appid);
	}
	std::string makeurl_steam_app_achievement_list_player(std::string_view api_key, std::string_view steamid, std::string_view appid)
	{
		return std::format(
			"{}?key={}&steamid={}&appid={}&l=en",
			my_urls::steap_app_achievement_list_player_baseurl,
			api_key,
			steamid,
			appid
		);
	}
	std::string makeurl_steam_player_summary(std::string_view api_key, std::string_view steamid)
	{
		return std::format(
			"{}?key={}&steamids={}&format=json",
			my_urls::steam_player_summary_baseurl,
			api_key,
			steamid
		);
	}
	std::string makeurl_steam_player_owned_games_list(std::string_view api_key, std::string_view steamid)
	{
		return std::format(
			"{}?key={}&steamid={}&include_appinfo=true&include_played_free_games=true&format=json",
			my_urls::steam_player_games_list_baseurl,
			api_key,
			steamid
		);
	}
	std::string makeurl_steam_player_activity(std::string_view api_key, std::string_view steamid)
	{
		return std::format(
			"{}?key={}&steamid={}&format=json",
			my_urls::steam_player_recent_activity_baseurl,
			api_key,
			steamid
		);
	}
}