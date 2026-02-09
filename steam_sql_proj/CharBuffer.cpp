#include "CharBuffer.h"
#include <iostream>
#include <conio.h>
namespace badSQL
{
	CharBuffer::CharBuffer()
	{
		static constexpr std::size_t MAX_BUFFER_SIZE = 256;
		buffer.set_capacity(MAX_BUFFER_SIZE);

		std::cout << "Enter password: ";

		for (;;) {
			char ch = _getch();

			if (ch == '\r') {
				break;
			}

			if (ch == '\b') {
				if (buffer.size() > 0) {
					buffer.pop_back();
					std::cout << "\b \b";
				}
			}
			else if (ch >= 32 && ch <= 126) {
				if (buffer.size() < MAX_BUFFER_SIZE) {
					buffer.emplace_back(ch);
					std::cout << '*';
				}
			}
			else {
				std::cout << '\a';
			}
		}
		buffer.emplace_back('\0');
		std::cout.flush();
	}
}