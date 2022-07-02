#include "../header/config.hpp"


Config::Config(){}

Config::~Config(){}

Config::Config(Config const &other)
{
    this->_servers = other._servers;
}

Config &Config::operator=(Config const &other)
{
    if (this != &other)
        this->_servers = other._servers;
    return *this;
}

std::vector<std::string> Config::readConfigFile(std::string const &fileName)
{
    std::vector<std::string> ret;
    size_t i(0);
    size_t j;
    std::ifstream file(fileName.c_str());

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
	file.close();
    return ret;
}

int     Config::parse(std::vector<std::string> const &configFile)
{
    for (size_t i = 0; i < configFile.size(); i++)
    {
        if (configFile[i++] == "server")
        {
            if (configFile[i++] == "{")
            {
                ConfigParse  server;
                
                if (server.parseConfigFile(i, configFile))
                {
                    std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
                    throw ConfigParse::InvalidArgsException();
                }
				_servers.push_back(server);   
            }
            else
            {
                std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
                throw ConfigParse::InvalidArgsException();
            }
        }
        else
        {
            std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
            throw ConfigParse::InvalidArgsException();
        }
    }
    return 0;
}

std::ostream	&operator<<(std::ostream &out, Config const &config)
{
	size_t i(1);
	for (std::vector<ConfigParse>::const_iterator it = config._servers.begin(); it != config._servers.end(); it++)
	{
		out << "[Server N°" << i++ << "]\n";
		out << *it << "---------------------------------------------\n";
	}
	return out;
} 
