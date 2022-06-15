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

bool isDirectory(std::string const &path) 
{
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
        return S_ISDIR(s.st_mode);
    return false;
}

bool isFile(std::string const &path) 
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
		return S_ISREG(s.st_mode);
	return false;
}

void removeDoubleSlash(std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '/' && str[i + 1] && str[i + 1] == '/')
		{
			while (str[++i] == '/')
				str.erase(str.begin() + i);
		}
	}
}

void replacePercent20BySpaces(std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '%' && str[i + 1] && str[i + 2] && str[i + 1] == '2' && str[i + 2] == '0')
		{
			str.replace(i, 3, " ");
		}
	}
}