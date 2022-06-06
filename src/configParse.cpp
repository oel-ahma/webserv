
#include "../header/configParse.hpp"

int ConfigParse::directiveArgsNumber(size_t const index, std::vector<std::string> const configFile, std::string *directive, ServerMap ParsedMap)
{
    size_t              argsNbr(0);

    if (ParsedMap.count(configFile[index]))
    {
        *directive = configFile[index];
        while (!ParsedMap.count(configFile[index + 1 + argsNbr]) && configFile[index + 1 + argsNbr] != "location" && configFile[index + 1 + argsNbr] != "}" && (index + 1 + argsNbr) != configFile.size())
            argsNbr++;
        return argsNbr;
    }
    return -1;
}

int ConfigParse::parseConfigFile(size_t &i, std::vector<std::string> const configFile)
{
    int                         argsNbr;
    std::vector<std::string>    args;
    std::string                 directive("");

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
            (this->*ConfigParse::_serverParsedMap[directive])(args);
            args.clear();
        }
        else
        {
            ConfigParse location;

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
                    (location.*ConfigParse::_locationParsedMap[directive])(args);
                    args.clear();
                }
            }
            _location[locationName] = location;
        }
    }
    return 0;
}

void    ConfigParse::addListen(std::vector<std::string> args)
{
	t_listen listen;
	if (args.size() != 2 || !isNumber(args[0]))
		throw ConfigParse::InvalidArgsException();
	listen.port = std::atoi(args[0].c_str());
	listen.host = args[1];
	_listen.push_back(listen);

}

void    ConfigParse::addRoot(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParse::InvalidArgsException();
    _root = args[0];
}

void    ConfigParse::addServerName(std::vector<std::string> args)
{
    if (args.size() == 0)
        throw ConfigParse::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
        _serverName.push_back(args[i]);
}

void    ConfigParse::addErrorPages(std::vector<std::string> args)
{
    if (args.size() != 2 || !isNumber(args[0]))
        throw ConfigParse::InvalidArgsException();
    _errorPages[std::atoi(args[0].c_str())] = args[1];
}

void    ConfigParse::addClientMaxBodySize(std::vector<std::string> args)
{
    if (args.size() != 1 || !isNumber(args[0]))
        throw ConfigParse::InvalidArgsException();
    _clientMaxBodySize = std::atoi(args[0].c_str());
}

void    ConfigParse::addAllowedMethods(std::vector<std::string> args)
{
    if (args.size() == 0)
        throw ConfigParse::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
        this->_allowedMethods.push_back(args[i]);

}

void    ConfigParse::addIndex(std::vector<std::string> args)
{
    if (args.size() == 0)
        throw ConfigParse::InvalidArgsException();
    for (size_t i = 0; i < args.size(); i++)
        _index.push_back(args[i]);    
}

void    ConfigParse::addAutoIndex(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParse::InvalidArgsException();
    if (args[0] == "off")
        _autoindex = false;
    else if (args[0] == "on")
        _autoindex = true;
    else
        throw ConfigParse::InvalidArgsException();

}

void    ConfigParse::addUploadEnable(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParse::InvalidArgsException();
    if (args[0] == "off")
        _uploadEnable = false;
    else if (args[0] == "on")
        _uploadEnable = true;
    else
        throw ConfigParse::InvalidArgsException();
}

void    ConfigParse::addUploadPath(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParse::InvalidArgsException();
    _uploadPath = args[0];
}


void    ConfigParse::addCgiPath(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParse::InvalidArgsException();
    _cgiPath = args[0];
}

void    ConfigParse::addCgiExtension(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParse::InvalidArgsException();
    _cgiExtension = args[0];
}

void    ConfigParse::addAlias(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ConfigParse::InvalidArgsException();
    _alias = args[0];

}

const char		*ConfigParse::InvalidArgsException::what()
 const throw() {
	return "Exception: Invalid Arguments In Configuration File";
}


void ConfigParse::initServerParsedMap()
{
    _serverParsedMap["listen"] = &ConfigParse::addListen;
    _serverParsedMap["root"] = &ConfigParse::addRoot;
    _serverParsedMap["server_name"] = &ConfigParse::addServerName;
    _serverParsedMap["error_page"] = &ConfigParse::addErrorPages;
    _serverParsedMap["client_max_body_size"] = &ConfigParse::addClientMaxBodySize;
    _serverParsedMap["method"] = &ConfigParse::addAllowedMethods;
    _serverParsedMap["index"] = &ConfigParse::addIndex;
    _serverParsedMap["autoindex"] = &ConfigParse::addAutoIndex;
    _serverParsedMap["cgi_path"] = &ConfigParse::addCgiPath;
    _serverParsedMap["cgi_extension"] = &ConfigParse::addCgiExtension;
    
    _serverParsedMap["upload_enable"] = &ConfigParse::addUploadEnable;
    _serverParsedMap["upload_path"] = &ConfigParse::addUploadPath;
}

void ConfigParse::initLocationParsedMap()
{
    _locationParsedMap["root"] = &ConfigParse::addRoot;
    _locationParsedMap["error_page"] = &ConfigParse::addErrorPages;
    _locationParsedMap["client_max_body_size"] = &ConfigParse::addClientMaxBodySize;
    _locationParsedMap["method"] = &ConfigParse::addAllowedMethods;
    _locationParsedMap["index"] = &ConfigParse::addIndex;
    _locationParsedMap["autoindex"] = &ConfigParse::addAutoIndex;
    _locationParsedMap["cgi_path"] = &ConfigParse::addCgiPath;
    _locationParsedMap["cgi_extension"] = &ConfigParse::addCgiExtension;
    _locationParsedMap["alias"] = &ConfigParse::addAlias;
    
    _locationParsedMap["upload_enable"] = &ConfigParse::addUploadEnable;
    _locationParsedMap["upload_path"] = &ConfigParse::addUploadPath;

}

std::ostream	&operator<<(std::ostream &out, ConfigParse const &server) 
{
    out << "server_name: ";
    for (size_t i = 0; i < server._serverName.size(); i++) {
        out << server._serverName[i];
        if (i != server._serverName.size() - 1)
            out << " ";
    }
    out << std::endl;
    out << "Listen: ";
    for (std::vector<t_listen>::const_iterator it = server._listen.begin(); it != server._listen.end(); it++) {
        out << "[port: " << (*it).port << " | host: " << (*it).host << "]" << std::endl;
    }
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
    for (std::map<std::string, ConfigParse>::const_iterator i = server._location.begin(); i != server._location.end(); i++) {
        out <<  "[" << i->first << "] \n" << i->second << std::endl;
    }
    out << std::endl;
    return out;
}

ConfigParse::ConfigParse() : _clientMaxBodySize(0), _root(""), _autoindex(false), _uploadEnable(false)
{
    initServerParsedMap();
    initLocationParsedMap();
}

ConfigParse::~ConfigParse(){}

ConfigParse::ConfigParse(ConfigParse const &other)
{
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

ConfigParse &ConfigParse::operator=(ConfigParse const &other)
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

std::vector<t_listen>           ConfigParse::getListen() const{ return _listen; }
std::string                     ConfigParse::getRoot() const { return _root; }
std::vector<std::string>        ConfigParse::getServerName() const { return _serverName; }
std::map<int, std::string>      ConfigParse::getErrorPages() const { return _errorPages; }
long                            ConfigParse::getClientMaxBodySize() const { return _clientMaxBodySize; }
std::vector<std::string>        ConfigParse::getAllowedMethods() const { return _allowedMethods; }
std::vector<std::string>        ConfigParse::getIndex() const { return _index; }
bool                            ConfigParse::getAutoindex() const { return _autoindex; }
bool                            ConfigParse::getUploadEnable() const { return _uploadEnable; }
std::string                     ConfigParse::getUploadPath() const { return _uploadPath; }
std::string                     ConfigParse::getCgiPath() const { return _cgiPath; } //it's not the right type of variable
std::string                     ConfigParse::getCgiExtension() const { return _cgiExtension; } //it's not the right type of variable
std::string                     ConfigParse::getAlias() const { return _alias; }