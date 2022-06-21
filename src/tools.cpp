#include "../header/header.hpp"
#include "../header/server.hpp"

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
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
			str.replace(i, 3, " ");
	}
}

int hostSyntaxCheck(std::string &str)
{
	size_t j = 3;
	std::string bits;
 	for (size_t i = 0; i < str.size(); i++)
	{
		while (std::isdigit(str[i]) && j > 0)
		{
			bits.push_back(str[i]);
			i++; j--;
		}
		if (std::atoi(bits.c_str()) > 255 || std::atoi(bits.c_str()) < 0)
			return 1;
		if (str[i] && str[i] != '.')
			return 1;
		j = 3;
		bits.clear();
	}
	return 0;
}