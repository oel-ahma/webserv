#ifndef _CONFIG_HPP
# define _CONFIG_HPP

#include "header.hpp"

class ConfigParse;

class Config
{
    public:
        Config();
        Config(Config const &other);
        ~Config();

        Config  &operator=(Config const &other);

        friend  std::ostream &operator<<(std::ostream &out, Config const &config);
        
        int                         parse(std::vector<std::string> const &configFile);
        std::vector<std::string>    readConfigFile(std::string const &fileName);

        std::vector<ConfigParse>    _servers;
    private:

};

#endif