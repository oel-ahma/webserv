#ifndef _CONFIGPARSE_HPP
# define _CONFIGPARSE_HPP


# define ServerMap std::map<std::string, void (ConfigParse::*)(std::vector<std::string>)>

#include "header.hpp"

class ConfigParse
{
    public:
        ConfigParse();
        ~ConfigParse();
        ConfigParse(ConfigParse const &other);

        int                      parseConfigFile(size_t &i, std::vector<std::string> const configFile);
        int                      directiveArgsNumber(size_t const index, std::vector<std::string> const configFile, std::string *directive, ServerMap ParsedMap);

        ConfigParse &operator=(ConfigParse const &other);
        class	InvalidArgsException : public std::exception 
        {
			virtual const char      *what() const throw();
		};

        friend	std::ostream &operator<<(std::ostream &out, ConfigParse const &server);

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

		//Getters
        std::vector<t_listen>           getListen();
        std::string                     getRoot();
        std::vector<std::string>        getServerName();
        std::map<int, std::string>      getErrorPages();
        long                            getClientMaxBodySize();
        std::vector<std::string>        getAllowedMethods();
        std::vector<std::string>        getIndex();
        bool                            getAutoindex();
        bool                            getUploadEnable();
        std::string                     getUploadPath();
        std::string                     getCgiPath(); //it's not the right type of variable
        std::string                     getCgiExtension(); //it's not the right type of variable
        std::string                     getAlias();		

        //Directives Members
        
        std::vector<t_listen>           _listen;
        long                            _clientMaxBodySize;
        std::string                     _root;
        std::vector<std::string>        _serverName;
        std::map<int, std::string>      _errorPages;
        std::vector<std::string>        _allowedMethods;
        std::vector<std::string>        _index;
        bool                            _autoindex;
        bool                            _uploadEnable;
        std::string                     _uploadPath;
        std::string                     _cgiPath; //it's not the right type of variable
        std::string                     _cgiExtension; //it's not the right type of variable
        std::string                     _alias;

        std::map<std::string, ConfigParse>   _location;
        //Parsing Members
        ServerMap                       _serverParsedMap;
        ServerMap                       _locationParsedMap;

};

#endif
