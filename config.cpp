#include "config.hpp"

Config::Config(){}

Config::~Config(){}

Config::Config(Config const &other){}

Config &Config::operator=(Config const &other)
{
	if (this != &other) {
		this->_listen = other._listen;
		this->_root = other._root;
		this->_serverName = other._serverName;
		this->_errorPages = other._errorPages;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_cgiPath = other._cgiPath;
		this->_cgiExtension = other._cgiExtension;
		this->_location = other._location;
		this->_allowedMethods = other._allowedMethods;
		this->_autoindex = other._autoindex;
		this->_index = other._index;
		this->_alias = other._alias;
	}
	return *this;
}

std::vector<std::string> Config::readConfigFile(std::string const &fileName)
{
    std::vector<std::string> ret;
    size_t i(0);
    size_t j;
    std::ifstream file(fileName);

    initServerParsedMap();
    initLocationParsedMap();
    (this->*_serverParsedMap["listen"])(ret);
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

void Config::initServerParsedMap()
{
    _serverParsedMap["listen"] = &Config::addListen;
    _serverParsedMap["root"] = &Config::addRoot;
    _serverParsedMap["server_name"] = &Config::addServerName;
    _serverParsedMap["error_page"] = &Config::addErrorPages;
    _serverParsedMap["client_max_body_size"] = &Config::addClientMaxBodySize;
    _serverParsedMap["method"] = &Config::addAllowedMethods;
    _serverParsedMap["index"] = &Config::addIndex;
    _serverParsedMap["autoindex"] = &Config::addAutoIndex;
    _serverParsedMap["cgi_path"] = &Config::addCgiPath;
    _serverParsedMap["cgi_extension"] = &Config::addCgiExtension;
    
    _serverParsedMap["upload_enable"] = &Config::addUploadEnable;
    _serverParsedMap["upload_path"] = &Config::addUploadPath;
}

void Config::initLocationParsedMap()
{
    _locationParsedMap["root"] = &Config::addRoot;
    _locationParsedMap["error_page"] = &Config::addErrorPages;
    _locationParsedMap["client_max_body_size"] = &Config::addClientMaxBodySize;
    _locationParsedMap["method"] = &Config::addAllowedMethods;
    _locationParsedMap["index"] = &Config::addIndex;
    _locationParsedMap["autoindex"] = &Config::addAutoIndex;
    _locationParsedMap["cgi_path"] = &Config::addCgiPath;
    _locationParsedMap["cgi_extension"] = &Config::addCgiExtension;
    _locationParsedMap["alias"] = &Config::addAlias;
    
    _locationParsedMap["upload_enable"] = &Config::addUploadEnable;
    _locationParsedMap["upload_path"] = &Config::addUploadPath;

}

int Config::directiveArgsNumber(size_t const index, std::vector<std::string> const configFile, std::string *directive, ServerMap ParsedMap)
{
    size_t              argsNbr = 0;
    ServerMap::iterator iter;

    if (ParsedMap.count(configFile[index]))
    {
        *directive = configFile[index];
        while (!ParsedMap.count(configFile[index + 1 + argsNbr]) && configFile[index + 1 + argsNbr] != "location" && configFile[index + 1 + argsNbr] != "}" && (index + 1 + argsNbr) != configFile.size())
            argsNbr++;
        return argsNbr;
    }
    return -1;
}


int Config::parseConfigFile(std::vector<std::string> const configFile)
{
    size_t                      i(0);
    int                         argsNbr;
    std::vector<std::string>    args;
    ServerMap::iterator         iter;
    std::string                 directive("");

    if (configFile[i++] == "server")
    {
        if (configFile[i++] == "{")
        {
            for (; i < configFile.size() && configFile[i] != "}" ; i++)
            {
                if (configFile[i] != "location")
                {
                    argsNbr = directiveArgsNumber(i, configFile, &directive, _serverParsedMap);
                    if (argsNbr == -1)
                    {
                        std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
                        return 1;
                    }
                    for (; argsNbr > 0; argsNbr--)
                        args.push_back(configFile[++i]);
                    (this->*Config::_serverParsedMap[directive])(args);
                    args.clear();
                }
                else
                {
                    Config location;

                    std::string locationName = configFile[++i];
                    
                    if (configFile[++i] == "{")
                    {
                        i++;
                        for (; i < configFile.size() && configFile[i] != "}" ; i++)
                        {
                            argsNbr = directiveArgsNumber(i, configFile, &directive, _locationParsedMap);
                            if (argsNbr == -1)
                            {
                                std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
                                return 1;
                            }
                            for (; argsNbr > 0; argsNbr--)
                                args.push_back(configFile[++i]);
                            (location.*Config::_locationParsedMap[directive])(args);
                            args.clear();
                        }
                    }
                    _location[locationName] = location;
                }
            }
            // if (i != configFile.size())
            //     std::cout << "index: " << i << " | " << configFile[i] << std::endl;
        }
        else
        {
            std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "Error Config File: Unkown Directive \"" << configFile[i] << "\"." << std::endl;
        return 1;
    }
    return 0;
}

void    Config::addListen(std::vector<std::string> args)
{
//    if (args.size() != 2)
//        throw Config::InvalidArgsException();
//
//
//

}

void    Config::addRoot(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw Config::InvalidArgsException();
    _root = args[0];
}

void    Config::addServerName(std::vector<std::string> args)
{
    if (args.size() == 0)
        throw Config::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
        _serverName.push_back(args[i]);
}

void    Config::addErrorPages(std::vector<std::string> args)
{
    if (args.size() != 2 || !isNumber(args[0]))
        throw Config::InvalidArgsException();
    _errorPages[std::atoi(args[0].c_str())] = args[1];
}

void    Config::addClientMaxBodySize(std::vector<std::string> args)
{
    if (args.size() != 1 || !isNumber(args[0]))
        throw Config::InvalidArgsException();
    _clientMaxBodySize = std::atoi(args[0].c_str());
}

void    Config::addAllowedMethods(std::vector<std::string> args)
{
    if (args.size() == 0)
        throw Config::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
        this->_allowedMethods.push_back(args[i]);

}

void    Config::addIndex(std::vector<std::string> args)
{
    if (args.size() == 0)
        throw Config::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
        _index.push_back(args[i]);    
}

void    Config::addAutoIndex(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw Config::InvalidArgsException();
    if (args[0] == "off")
        _autoindex = false;
    else if (args[0] == "on")
        _autoindex = true;
    else
        throw Config::InvalidArgsException();

}

void    Config::addUploadEnable(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw Config::InvalidArgsException();
    if (args[0] == "off")
        _uploadEnable = false;
    else if (args[0] == "on")
        _uploadEnable = true;
    else
        throw Config::InvalidArgsException();
}

void    Config::addUploadPath(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw Config::InvalidArgsException();
    _uploadPath = args[0];
}


void    Config::addCgiPath(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw Config::InvalidArgsException();
    _cgiPath = args[0];
}

void    Config::addCgiExtension(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw Config::InvalidArgsException();
    _cgiExtension = args[0];
}

void    Config::addAlias(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw Config::InvalidArgsException();
    _alias = args[0];

}

const char		*Config::InvalidArgsException::what()
 const throw() {
	return "Exception: Invalid Arguments In Configuration File";
}

std::ostream	&operator<<(std::ostream &out, Config const &server) {
	out << "server_name: ";
	for (size_t i = 0; i < server._serverName.size(); i++) {
		out << server._serverName[i];
		if (i != server._serverName.size() - 1)
			out << " ";
	}
	out << "Listen: \"--------\"" << std::endl;
	out << "root: " << server._root << std::endl;
	out << std::endl;
    out << "error_page: \n";
	for (std::map<int, std::string>::const_iterator i = server._errorPages.begin(); i != server._errorPages.end(); i++) {
		out <<  i->first << " | " << i->second << std::endl;
	}
    out << std::endl;
	out << "client_body_buffer_size: " << server._clientMaxBodySize << std::endl;
	out << "cgi_path:" << server._cgiPath << std::endl;
	out << "cgi_Extension:	" << server._cgiExtension << std::endl;
	out << "allowed methods:";
	for (std::vector<std::string>::const_iterator i = server._allowedMethods.begin(); i != server._allowedMethods.end(); i++)
		out << " " << *i;
	out << std::endl;
	out << "autoindex: " << server._autoindex << std::endl;
	out << "index: ";
	for (std::vector<std::string>::const_iterator i = server._index.begin(); i != server._index.end(); i++)
		out << *i << " ";
	out << std::endl;
	out << "alias: " << server._alias << std::endl;
	out << "locations: ";
	for (std::map<std::string, Config>::const_iterator i = server._location.begin(); i != server._location.end(); i++) {
		out <<  "[" << i->first << "] \n" << i->second << std::endl;
	}
    out << std::endl;
    return out;
}

int main(int ac, char **av)
{
    Config _config;
    std::vector<std::string> configFile;
    if (ac != 2)
    {
        std::cerr << "Not Enough Argument!" << std::endl;
        return 1;
    }
    configFile = _config.readConfigFile(av[1]);
    try {
        _config.parseConfigFile(configFile);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
	}
    std::cout << _config << std::endl;
    return 0;
}