#pragma once
#include <iostream>
#include <string>
#include <conio.h>
#include "Sequence.h"

namespace badSQL
{    
    Sequence<char> input_password() 
    {    
        constexpr std::size_t MAX_PASSWORD_LEN = 256;
        Sequence<char> password;
        password.set_capacity(64);
        char ch;

        std::cout << "Enter password: ";
        while ((ch = _getch()) != '\r') {

            if (ch == '\b' && !password.isEmpty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
            else if (ch >= 32 && ch <= 126) {
                if (password.size() < 256) {
                    password.emplace_back(ch);
                    std::cout << '*';
                }
            }
            else {
                std::cout << '\a';//beet boop sound BEL
            }
        }
        password.emplace_back('\0');
        std::cout.flush();
        return password;
    }

    std::string input() {
        std::string str;
        std::getline(std::cin, str);
        return str;
    }
}