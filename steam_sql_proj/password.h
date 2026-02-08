#pragma once

#include "Sequence.h"

namespace badSQL
{

	class CharBuffer
	{

		void input() {

		}
		void clear() {

		}
		char* data() {
			return buffer.data();
		}
		std::size_t size()const {
			return buffer.size();
		}

	private:
		Sequence<char> buffer;
	};
}