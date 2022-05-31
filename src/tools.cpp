#include "../header/header.hpp"
#include "../header/server.hpp"

bool isNumber(const std::string& s)
{
    for (char const &ch : s) {
        if (std::isdigit(ch) == 0) 
            return false;
    }
    return true;
}