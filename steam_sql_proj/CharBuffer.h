#pragma once

#include <algorithm>
#include "Sequence.h"

namespace badSQL
{
	class CharBuffer
	{
	public:
		CharBuffer();
		CharBuffer(const CharBuffer&) = delete;
		CharBuffer& operator=(const CharBuffer&) = delete;
		CharBuffer(CharBuffer&&) = delete;
		CharBuffer& operator=(CharBuffer&&) = delete;

		char* data()noexcept {
			return buffer.data();
		}

		std::size_t size()const noexcept
		{
			return buffer.size();
		}

		~CharBuffer()
		{
			std::fill(buffer.begin(), buffer.end(), '\0');
			buffer.wipe();
		}

	private:
		Sequence<char> buffer;
	};
}