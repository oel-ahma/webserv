#include "webserv.hpp"

template<typename T>
void print_vector(std::vector<T> const &vec)
{
for (auto i = vec.begin(); i != vec.end(); ++i)
    std::cout << *i << "\n";
}

bool isNumber(const std::string& s)
{
    for (char const &ch : s) {
        if (std::isdigit(ch) == 0) 
            return false;
    }
    return true;
}
