// #include "request.hpp"
#include "../header/request.hpp" //TODO:

void	Request::CreatetTmpFile() {
	this->file_name = "/tmp/request_XXXXXX";
	this->file_fd = mkstemp(&(*file_name.begin()));
	if (this->file_fd == ERROR)
			throw std::runtime_error("mkstemp(): " + (std::string)strerror(errno));
	this->file_stream.open(this->file_name.c_str(), std::fstream::in | std::fstream::out);
	if (this->file_stream.fail())
		throw std::ios_base::failure("failed to open file: " + this->file_name);
	// std::cout << "file name of socket " << this->ClientSocket << " is " << this->file_name << std::endl; //DEBUG
}

Request::Request(std::string const &str) : 
    _body(""), _statusCode(200), _rawString(str)
{
    parsing(*this);
    if (this->_statusCode != 200)
        std::cerr << "Parsing Error : " << this->_statusCode << std::endl;
}

void Request::parsing(Request & client)
{
	char			*str;
    std::string		line;
	std::string     key;
    std::string     value;
    size_t			j;
    size_t          i(0);

	initHeaders();
    initMethodList();
	this->_statusCode = 200;
	this->file_stream.getline(str, BUFF+100);//TODO:
	parseFirstLine(gnl(str, i));
	this->file_stream.getline(str, BUFF+100);//TODO: ne pas extract
    while(((line = gnl(str, i)) != "") && _statusCode != 400)
	{
        j = line.find_first_of(':');
        key = line.substr(0, j);
        value = line.substr(j + 2, std::string::npos);
        //if (_headers.count(key))
		_headers[key] = value;
    }
	_body = str.substr(i, std::string::npos);
}

std::string Request::gnl(std::string const &str, size_t &i)
{
    std::string ret;
    size_t      j;

    if (i == std::string::npos)
        return "";
    ret = str.substr(i, std::string::npos);
	j = ret.find_first_of('\n');
    ret = ret.substr(0, j);
    if (ret[ret.size() - 1] == '\r')
	{
        ret.resize(ret.size() - 1);
	}
	i = ((j == std::string::npos) ? j : j + i + 1);
    return ret;
}

void    Request::parseFirstLine(std::string const &str)
{
    size_t i(0);
    std::string line;

    //GET METHOD
    i = str.find_first_of(' ');
    if (i == std::string::npos)
    {
        std::cerr << "No Space After Method" << std::endl;
        _statusCode = 400;
        return ;
    }
    _method.assign(str, 0, i);
    size_t j(0);
    while (_method != _methodList[j++] && j < _methodList.size());
    if (j >= _methodList.size())
    {
        std::cerr << "Invalid Method Request" << std::endl;
        _statusCode = 400;
        return ;
    }
    //GET PATH
    line = str.substr(i + 1, std::string::npos);
    i = line.find_first_not_of(' ');
    if (i == std::string::npos)
    {
        std::cerr << "No Path / Version" << std::endl;
        _statusCode = 400;
        return ;
    }
    i = line.find_first_of(' ');
    if (i == std::string::npos)
    {
        std::cerr << "No Version" << std::endl;
        _statusCode = 400;
        return ;
    }
    _path.assign(line, 0, i);
    //GET QUERY
    j = _path.find_first_of('?');
    if (j != std::string::npos)
    {
        _query = _path.substr(j + 1, std::string::npos);
        _path = _path.substr(0, j);
    }
    //GET VERSION
    line = line.substr(i + 1, std::string::npos);
    if ((line.compare(0, 8, "HTTP/1.1") == 0 || line.compare(0, 8, "HTTP/1.0") == 0 ) && line.size() == 8)
        _version.assign(line, 0, 8);
    else
    {
        std::cerr << "Bad HTTP version" << std::endl;
        _statusCode = 400;
        return ;
    }
}

void Request::initMethodList() {
    _methodList.push_back("GET");
	_methodList.push_back("DELETE");
	_methodList.push_back("POST");
	// Optional Methods
	_methodList.push_back("HEAD");
	_methodList.push_back("PUT");
	_methodList.push_back("OPTIONS");
	_methodList.push_back("TRACE" );

}

void Request::initHeaders() {
    this->_headers.clear();
}

Request::Request() : _statusCode(200), sizeBuff(0), file_fd(-1) {
	memset(buff, 0, BUFF);
}

Request::~Request(){}

Request::Request(Request const &other) {
	*this = other;
}

Request &Request::operator=(Request const &rhs) {
	if (this == &rhs)
		return (*this);
    this->_method = rhs._method;
    this->_path = rhs._path;
    this->_query = rhs._query;
    this->_version = rhs._version;
    this->_methodList = rhs._methodList;
    this->_headers = rhs._headers;
    this->_body = rhs._body;
    this->_statusCode = rhs._statusCode;
    this->_rawString = rhs._rawString;
	this->sizeBuff = rhs.sizeBuff;
	this->file_name = rhs.file_name;
	this->file_fd = rhs.file_fd;
	this->ClientSocket = rhs.ClientSocket;
	strcpy(this->buff, rhs.buff);
    return (*this);
}

void								Request::setStatusCode(size_t statusCode) { this->_statusCode = statusCode; }

//Getters
std::map<std::string, std::string>  Request::getHeaders() const { return this->_headers; }
std::string                         Request::getBody() const { return this->_body; }
size_t                              Request::getStatusCode() const { return this->_statusCode; }
std::string                         Request::getRawString() const {return this->_rawString; }
std::string							Request::getMethod() const { return this->_method; }
std::string							Request::getQuery() const { return this->_query; }
std::string							Request::getPath() const {return this->_path; }
std::string							Request::getVersion() const { return this->_version; }

