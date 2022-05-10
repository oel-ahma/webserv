#ifndef _CONFIG_HPP
# define _CONFIG_HPP

#include "server.hpp"

# define ServerMap std::map<std::string, void (Config::*)(std::vector<std::string>)>


class Config
{
    public:
        Config();
        ~Config();
        Config(Config const &other);

        std::vector<std::string> readConfigFile(std::string const &fileName);
        int                      parseConfigFile(std::vector<std::string> const configFile);
        int                      directiveArgsNumber(size_t const index, std::vector<std::string> const configFile, std::string *directive, ServerMap ParsedMap);

        Config &operator=(Config const &other);
        class	InvalidArgsException : public std::exception 
        {
			virtual const char      *what() const throw();
		};

        friend	std::ostream &operator<<(std::ostream &out, Config const &server);

    private:
        //Init Functions
        void                        initServerParsedMap();
        void                        initLocationParsedMap();



        //Add Server Directives Functions
        void                        addListen(std::vector<std::string> args);
        void                        addRoot(std::vector<std::string> args);
        void                        addServerName(std::vector<std::string> args);
        void                        addErrorPages(std::vector<std::string> args);
        void                        addClientMaxBodySize(std::vector<std::string> args);
        void                        addAllowedMethods(std::vector<std::string> args);
        void                        addIndex(std::vector<std::string> args);
        void                        addAutoIndex(std::vector<std::string> args);
        void                        addCgiPath(std::vector<std::string> args);
        void                        addCgiExtension(std::vector<std::string> args);
        void                        addAlias(std::vector<std::string> args);
        void                        addUploadEnable(std::vector<std::string> args);
        void                        addUploadPath(std::vector<std::string> args);


        //Directives Members
        
        std::size_t                     _listen; //it's not the right type of variable
        std::string                     _root;
        std::vector<std::string>        _serverName;
        std::map<int, std::string>      _errorPages;
        long                            _clientMaxBodySize;
        std::vector<std::string>        _allowedMethods;
        std::vector<std::string>        _index;
        bool                            _autoindex;
        bool                            _uploadEnable;
        std::string                     _uploadPath;
        std::string                     _cgiPath; //it's not the right type of variable
        std::string                     _cgiExtension; //it's not the right type of variable
        std::string                     _alias;

        std::map<std::string, Config>   _location;
        //Parsing Members
        ServerMap                   _serverParsedMap;
        ServerMap                   _locationParsedMap;

};

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

#endif