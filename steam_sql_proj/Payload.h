#pragma once

#include <optional>
#include <string>
#include "json.hpp"

namespace badSQL
{
	struct Payload {
		std::optional<std::size_t> recipient_id;
		std::optional<std::string> label;
		std::optional<nlohmann::json> data;//data = {} does not initalize the json, it calls the std::optional constructor... a pitfall to remember
	};
}