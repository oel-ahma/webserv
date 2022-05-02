#include "request.hpp"

Request::Request(std::string const &str) : 
    _body(""), _statusCode(200), _rawString(str)
{
    initHeaders();
    print_map(this->_headers);
    parsing(str);
    if (this->_statusCode != 200)
        std::cerr << "Parsing Error : " << this->_statusCode << std::endl;
}

void Request::parsing(std::string const &str)
{
    std::string		ret;
	size_t			j;
    size_t          i(0);

	j = str.find_first_of('\n', i);
	ret = str.substr(i, j - i);
//	if (ret[ret.size() - 1] == '\r')
//		pop(ret);
	i = (j == std::string::npos ? j : j + 1);
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
    std::cout << buff << "\n---------------------------\n";
    Request request(buff);
    return 0;
}