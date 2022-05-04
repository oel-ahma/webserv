#include "config.hpp"

Config::Config(){}

Config::~Config(){}

Config::Config(Config const &other){}

std::vector<std::string> Config::readConfigFile(std::string const &fileName)
{
    std::vector<std::string> ret;
    size_t i(0);
    size_t j;
    std::ifstream file(fileName);
    if (!file.fail())
    {
        std::string buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        while (i != std::string::npos && j != std::string::npos)
        {
            i = buff.find_first_not_of(" \t\n");
            j = buff.find_first_of(" \t\n", i);
            if (j != std::string::npos)
            {
                ret.push_back(buff.substr(i, j - i));
                buff = buff.substr(j, std::string::npos);
            }
            else
                ret.push_back(buff.substr(i, std::string::npos - 1));
        }
    }
    return ret;
}

int Config::parseConfigFile(std::vector<std::string> const configFile)
{
    for (size_t i = 0; i < configFile.size(); i++)
    {
        if (configFile[i] == "server")
        {
            if (configFile[++i] != "{")
            {
                
            }
            if (conf)

        }
        else
        {
            std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
            return 1;
        }
    }
}

int main(int ac, char **av)
{
    Config _config;
    if (ac != 2)
    {
        std::cerr << "Not Enough Argument!" << std::endl;
        return 1;
    }
    _config.readConfigFile(av[1]);
    return 0;
}