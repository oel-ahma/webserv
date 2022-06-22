// #include "request.hpp"
#include "../header/request.hpp" //TODO:



void Request::parsing()
{
    std::string		line;
	std::string     key;
    std::string     value;
    size_t			j;
    size_t          i(0);

	initHeaders();
    initMethodList();
	this->_statusCode = 200;
	parseFirstLine(gnl(_request, i));
    while(((line = gnl(_request, i)) != "") && _statusCode != 400)
    {
        j = line.find_first_of(':');
        key = line.substr(0, j);
        value = line.substr(j + 2, std::string::npos);
        //if (_headers.count(key))
		_headers[key] = value;
    }
	if (_statusCode == 200)
		_body = _request.substr(i, std::string::npos);
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
		ret.resize(ret.size() - 1);
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
	replacePercent20BySpaces(_path);
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

Request::Request() : _statusCode(200), sizeBuff(0) {
	memset(buff, 0, BUFF);
}
//Check THAT v
Request::Request(std::string const &str) : 
    _body(""), _statusCode(200), _rawString(str)
{
    parsing();
    if (this->_statusCode != 200)
        std::cerr << "Parsing Error : " << this->_statusCode << std::endl;
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
	this->_request = rhs._request;
	this->sizeBuff = rhs.sizeBuff;
	this->ClientSocket = rhs.ClientSocket;
	strcpy(this->buff, rhs.buff);
    return (*this);
}

void			Request::clearAll()
{
	_method.clear();
	_path.clear();
	_query.clear();
	_version.clear();
	_methodList.clear();
	_headers.clear();
	_body.clear();
	_statusCode = 200;
	_rawString.clear();
	_request.clear();
	sizeBuff = 0;
	ClientSocket = 0;
}

void								Request::setStatusCode(size_t statusCode) { this->_statusCode = statusCode; }
void								Request::setVersion(std::string const &str) { this->_version = str; }
void								Request::setRequest(char *buff, int size) { 
	/*this->_statusCode = 200;*/
this->_request.append(buff, size); }

//Getters
std::map<std::string, std::string>  Request::getHeaders() const { return this->_headers; }
std::string                         Request::getBody() const { return this->_body; }
size_t                              Request::getStatusCode() const { return this->_statusCode; }
std::string                         Request::getRawString() const {return this->_rawString; }
std::string							Request::getMethod() const { return this->_method; }
std::string							Request::getQuery() const { return this->_query; }
std::string							Request::getPath() const {return this->_path; }
std::string							Request::getVersion() const { return this->_version; }
std::string							Request::getRequest() const { return this->_request; }


