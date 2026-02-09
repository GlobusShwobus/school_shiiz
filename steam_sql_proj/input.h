#pragma once

#include <string>
#include "Sequence.h"

namespace badSQL
{    
    std::string input() {
        std::string str;
        std::getline(std::cin, str);
        return str;
    }
}