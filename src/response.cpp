// #include "Response.hpp"
#include "../header/response.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Response::Response() : sizeBuff(0), responseIsSet(false), file_fd(-1), _headers(""), _body("") {
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
	return (*this);
}

/* ************************************************************************** */
/*                                 ACCESSORS                                  */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                  SETTERS	                                  */
/* ************************************************************************** */
void	setRequest(Request *other) {
	(void)other;
	//this->_request = other;
}
/* ************************************************************************** */
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */


void	Response::prepareResponse(Request *request) {
	if (this->file_stream.is_open() == false) {
		// std::cout << "creation du fichier pour l'envoi." << std::endl;
		this->CreateTmpFile();
	}
	//Generate A Response From Request.
	this->createResponse(request);
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

void Response::createResponse(Request *request) {
	this->_request = *request;
	if (this->_request.getMethod() == "GET")
		this->httpGetMethod();
	if (this->_request.getMethod() == "POST")
		this->httpPostMethod();
}

int	Response::httpGetMethod() {
	std::string tmp_path;
	std::fstream file;

	tmp_path = "." + this->_request.getRoot() + this->_request.getPath();
	if (this->_request.getPath() == "/")
		tmp_path += this->_request.getIndex();
	else
		tmp_path += ".html";
	file.open(tmp_path, std::fstream::in | std::fstream::out);
	if (!file.fail())
	{
		std::string buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		this->_body = buff + "\r\n";
	}
	else
	{
		this->_body = "<!doctype html><html><head><title>404 Page Not Found!</title><h1><b>Error 404</b></h1><h2>Page Not Found</h2></head></html>\r\n";
		this->_request.setStatusCode(404);
	}
	file.close();
	this->_statusLine = this->_request.getVersion() + " " + std::to_string(this->_request.getStatusCode()) + " " + this->_statusMsg[this->_request.getStatusCode()] + "\r\n";
	this->setHeaders();
	this->_response = this->_statusLine + this->_headers + this->_body;
	return 0;
}

int Response::httpPostMethod() {
	std::cout << "The Body is: "<< this->_request.getBody() << std::endl;
	std::cout << "The Headers are: ";
	print_map(this->_request.getHeaders());
	this->_response = "HTTP/1.1 200 OK\r\n\r\n";
	return 0;
}

int		Response::setHeaders() {
	//TODO: Which Headers to Put in...
	this->_headers = "\r\n\r\n";
	return 0;
}

void	Response::initStatusCodeMsg() {
	_statusMsg[200] = "OK";
	_statusMsg[400] = "BAD REQUEST";
	_statusMsg[403] = "FORBIDDEN";
	_statusMsg[404] = "NOT FOUND";
	_statusMsg[415] = "UNSUPPORTED MEDIA TYPE";
	_statusMsg[500] = "INTERNAL SERVER ERROR";
	_statusMsg[502] = "BAD GATEWAY";
}
/* ************************************************************************** */
/*                             NO MEMBER FUNCTION                             */
/* ************************************************************************** */
