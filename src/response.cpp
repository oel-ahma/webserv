// #include "Response.hpp"
#include "../header/response.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Response::Response() : sizeBuff(0), responseIsSet(false), file_fd(-1), _responseLocation(NULL) {
	memset(buff, 0, BUFF);
	this->initStatusCodeMsg();
}

Response::Response(Response const &cpy) {
	*this = cpy;
}

/* ************************************************************************** */
/*                                 DESTRUCTOR                                 */
/* ************************************************************************** */

Response::~Response() {
}

/* ************************************************************************** */
/*                                 OPERATORS                                  */
/* ************************************************************************** */

Response &Response::operator=(Response const &other) {
	if (this == &other)
		return (*this);
	this->_request = other._request;
	this->_config = other._config;
	this->_statusMsg = other._statusMsg;
	this->_statusLine = other._statusLine;
	this->_headers = other._headers;
	this->_body = other._body;
	this->_response = other._response;

	return (*this);
}

/* ************************************************************************** */
/*                                 ACCESSORS                                  */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                  SETTERS	                                  */
/* ************************************************************************** */

/* ************************************************************************** */
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */


void	Response::prepareResponse(Request *request, ConfigParse const *config) {
	this->_request = request;
	this->_config = config;

	if (this->_request->getBody().size() > this->_config->getClientMaxBodySize())
		this->_request->setStatusCode(413);
	if (this->file_stream.is_open() == false) {
		// std::cout << "creation du fichier pour l'envoi." << std::endl;
		this->CreateTmpFile();
	}
	//Generate A Response From Request.
	this->createResponse();
	//Put The Response In the File.
	this->file_stream << this->_response;
	//Fill The Buffer From the File.
	this->file_stream.close();
	this->file_stream.open(this->file_name.c_str(), std::fstream::in | std::fstream::out);
	this->file_stream.read(this->buff, BUFF);//TODO: read() ne semble pas fonctionner. il read 0 char. Pourquoi ?
	if (this->file_stream.bad())
			throw std::ios_base::failure("failed to read file: " + this->file_name);
	this->sizeBuff = this->file_stream.gcount();
	// std::cout << "My response buffer contains:\n" << this->buff << std::endl;
	// std::cout << "sizeBuff = " << this->sizeBuff << std::endl;
	this->responseIsSet = true;
}

void	Response::CreateTmpFile() {
	this->file_name = "/tmp/response_XXXXXX";
	this->file_fd = mkstemp(&(*file_name.begin()));
	if (this->file_fd == ERROR)
			throw std::runtime_error("mkstemp(): " + (std::string)strerror(errno));
	this->file_stream.open(this->file_name.c_str(), std::fstream::in | std::fstream::out);
	if (this->file_stream.fail())
		throw std::ios_base::failure("failed to open file: " + this->file_name);
	// std::cout << "fd " << this->file_fd << " is created as " << this->file_name << std::endl; //DEBUG
}

void Response::createResponse() {
	setResponseLocation();
	if (this->_request->getMethod() == "GET")
		this->httpGetMethod();
	if (this->_request->getMethod() == "POST")
		this->httpPostMethod();
	if (this->_request->getStatusCode() != 200)
	{
		std::fstream errfile;
		std::string error_path;
		error_path = this->_config->getErrorPages()[this->_request->getStatusCode()];
		errfile.open(error_path, std::fstream::in | std::fstream::out);
		if (!errfile.fail())
		{
			std::string errbuff((std::istreambuf_iterator<char>(errfile)), std::istreambuf_iterator<char>());
			this->_body = errbuff + "\r\n";
		}
		else
		{
				this->_request->setStatusCode(500);
				this->_body = "<!doctype html><html><head><title>500 Internal Server Error</title><h1><b>Error 500</b></h1><h2>Internal Server Error</h2></head></html>\r\n";
		}
		errfile.close();
	}
	this->_statusLine = this->_request->getVersion() + " " + std::to_string(this->_request->getStatusCode()) + " " + this->_statusMsg[this->_request->getStatusCode()] + "\r\n";
	this->setHeaders();
	this->_response = this->_statusLine + this->_headers + this->_body;
}

int	Response::httpGetMethod() {
	std::string tmp_path;
	std::fstream file;

	tmp_path = "." + this->_config->getRoot() + this->_request->getPath();
	if (this->_request->getPath() == "/")
		tmp_path += this->_config->getIndex().at(0);
	/*else
		tmp_path += ".html";*/
	file.open(tmp_path, std::fstream::in | std::fstream::out);
	if (!file.fail())
	{
		std::string buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		this->_body = buff + "\r\n";
	}
	else
		this->_request->setStatusCode(404);
	file.close();
	return 0;
}

int Response::httpPostMethod() {
	std::cout << YELLOW <<  "The Body is: "<< this->_request->getBody() << RESET << std::endl;
	return 0;
}

int		Response::setHeaders() {
	//TODO: Which Headers to Put in...
	this->_headers += "Version: " + this->_request->getVersion() + "\r\n";
	this->_headers += "Connection: keep-alive\r\n";
	// this->_headers += "Keep-Alive: timeout=10\r\n";
	this->_headers += "Content-Length: " + std::to_string(this->_body.length());
	this->_headers += "\r\n\r\n";

	return 0;
}

void	Response::setResponseLocation() {
	std::map<std::string, ConfigParse>::const_iterator it;
	std::string tmp = this->_request->getPath();

	// TMP = "/1.html"
	// TMP = "/inside/3.html"
	while (1)
	{
		it = this->_config->getLocation().find(tmp);
		print_map(this->_config->getLocation());
		if (it != this->_config->getLocation().end())
		{
			std::cout << RED << "yo" << RESET << std::endl;
			this->_responseLocation = &(it->second);
			return ;
		}
		std::cout << RED << tmp << RESET << std::endl;
		if (tmp == "/")
		{
			this->_request->setStatusCode(404);
			return ;
		}
		tmp = tmp.substr(0, tmp.rfind("/"));
		std::cout << RED << tmp << RESET << std::endl;
		if (tmp.empty())
			tmp = "/";
	}
	if (this->_responseLocation)
	{
		std::cout << RED << "heps"; 
		print_vector(this->_responseLocation->getAllowedMethods());
		std::cout << RESET;
	}
}

void	Response::initStatusCodeMsg() {
	_statusMsg[200] = "OK";
	_statusMsg[400] = "BAD REQUEST";
	_statusMsg[403] = "FORBIDDEN";
	_statusMsg[404] = "NOT FOUND";
	_statusMsg[413] = "PAYLOAD TOO LARGE";
	_statusMsg[415] = "UNSUPPORTED MEDIA TYPE";
	_statusMsg[500] = "INTERNAL SERVER ERROR";
	_statusMsg[502] = "BAD GATEWAY";
}
/* ************************************************************************** */
/*                             NO MEMBER FUNCTION                             */
/* ************************************************************************** */
