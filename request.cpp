#include "request.hpp"

Request::Request(std::string const &str) : 
    _body(""), _statusCode(200), _rawString(str)
{
    initHeaders();
    initMethodList();
    print_map(this->_headers);
    parsing(str);
    print_map(this->_headers);
    if (this->_statusCode != 200)
        std::cerr << "Parsing Error : " << this->_statusCode << std::endl;
}

void Request::parsing(std::string const &str)
{
    std::string		line;
	std::string     key;
    std::string     value;
    size_t			j;
    size_t          i(0);

    parseFirstLine(gnl(str, i));
    while(((line = gnl(str, i) )!= "") && _statusCode != 400)
    {
        j = line.find_first_of(':');
        key = line.substr(0, j);
        value = line.substr(j + 1, line.size());
        if (_headers.count(key))
            _headers[key] = value; 
    }

}

std::string Request::gnl(std::string const &str, size_t &i)
{
    std::string ret;
    size_t      j;

    if (i == std::string::npos)
        return "";
    ret = str.substr(i, str.size());
    j = ret.find_first_of('\n');
    ret = ret.substr(0, j);
    //need to fix 'cuz didn't took '\r' in consideration
    //	if (ret[ret.size() - 1] == '\r')
    //		pop(ret);
	i = (j == std::string::npos ? j : j + i + 1);
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
    line = str.substr(i + 1, str.size());
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
        _query = _path.substr(j + 1, _path.size());
        _path = _path.substr(0, j);
    }
    //GET VERSION
    line = line.substr(i + 1, line.size());
    if ((line.compare(0, 8, "HTTP/1.1") == 0 || line.compare(0, 8, "HTTP/1.0") == 0 ) && line.size() == 8)
        _version.assign(line, 0, i);
    else
    {
        std::cerr << "Bad HTTP version" << std::endl;
        _statusCode = 400;
        return ;
    }
}

void Request::initMethodList()
{
    _methodList.push_back("GET");
	_methodList.push_back("HEAD");
	_methodList.push_back("POST");
	_methodList.push_back("PUT");
	_methodList.push_back("DELETE");
	_methodList.push_back("OPTIONS");
	_methodList.push_back("TRACE" );

}

void Request::initHeaders()
{
    this->_headers.clear();

    this->_headers["Accept"] = "";
	this->_headers["Accept-Charsets"] = "";
	this->_headers["Accept-Language"] = "";
	this->_headers["Allow"] = ""; 
	this->_headers["Auth-Scheme"] = ""; 
	this->_headers["Authorization"] = "";
	this->_headers["Content-Language"] = "";
	this->_headers["Content-Length"] = "";
	this->_headers["Content-Location"] = "";
	this->_headers["Content-Type"] = "";
	this->_headers["Date"] = "";
	this->_headers["Host"] = "";
	this->_headers["Last-Modified"] = "";
	this->_headers["Location"] = "";
	this->_headers["Referer"] = "";
	this->_headers["Retry-After"] = "";
	this->_headers["Server"] = "";
	this->_headers["Transfer-Encoding"] = "";
	this->_headers["User-Agent"] = "";
	this->_headers["Www-Authenticate"] = "";
	this->_headers["Connection"] = "Keep-Alive";
}

Request::Request(){}

Request::~Request(){}

Request::Request(Request const &other)
{
    this->_method = other._method;
    this->_path = other._path;
    this->_version = other._version;
    this->_headers = other._headers;
    this->_body = other._body;
    this->_statusCode = other._statusCode;
    this->_rawString = other._rawString;
}

Request &Request::operator=(Request const &rhs)
{
    this->_headers = rhs._headers;
    this->_body = rhs._body;
    this->_statusCode = rhs._statusCode;
    this->_rawString = rhs._rawString;
    return *this;
}

int main(int ac, char **av)
{

    if (ac != 2)
    {
        std::cout << "Not Enough Arguments !" << std::endl;
        return 1;
    }
    std::ifstream t(av[1]);
    std::string buff((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    //std::cout << buff << "\n---------------------------\n";
    Request request(buff);
    return 0;
}