#pragma once
#include <string>
#include "Sequence.h"

namespace badSQL
{
	struct WebRequestHandle {
		Sequence<char> data;
		long httpcode = -1;
		bool is_good = false;
		std::string contentType;
	};
}