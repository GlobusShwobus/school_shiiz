#include "bad_exceptions.h"

namespace badSQL
{
	BadException::BadException(const char* my_message, const char* extra)
		:mDescription(my_message)
	{
		if (extra) {
			mDescription += " [[ ";
			mDescription += extra;
			mDescription += " ]]";
		}
	}

	const char* BadException::what() const noexcept
	{
		return mDescription.c_str();
	}

	const std::string& BadException::message()const noexcept
	{
		return mDescription;
	}
}