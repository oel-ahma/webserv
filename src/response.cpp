// #include "Response.hpp"
#include "../header/response.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Response::Response() : sizeBuff(0), responseIsSet(false), file_fd(-1) {
	memset(buff, 0, BUFF);
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
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */


void	Response::prepareResponse() {
	// this->buff = "HTTP/1.1 201 OK\r\nDate: Sun, 18 Oct 2012 10:36:20 GMT\r\n\r\n<!DOCTYPE html><html><head><title>Example</title></head><body><p>This is an example of a simple HTML page with one paragraph.</p></body></html>\r\n";
	std::cout << "send fct" << std::endl;
	if (this->file_stream.is_open() == false) {
		std::cout << "creation du fichier pour l'envoi." << std::endl;
		this->CreateTmpFile();
	}
	//Parser la requete pour creer une reponse
	//mettre la reponse dans le fichier
	//remplir le buffer de la reponse
	// this->bodyStream.read(&this->sendBuf[header_len], BUF_SIZE - header_len);
	this->file_stream.read(this->buff, BUFF);
	// if (this->file_stream.fail())
			// throw std::ios_base::failure("failed to read file: " + this->file_name);
	this->responseIsSet = true;
}

void	Response::CreateTmpFile() {
	this->file_name = "/tmp/fileXXXXXX";
	this->file_fd = mkstemp(&(*file_name.begin()));
	if (this->file_fd == ERROR)
			throw std::runtime_error("mkstemp(): " + (std::string)strerror(errno));
	this->file_stream.open(this->file_name.c_str(), std::fstream::in | std::fstream::out);
	if (this->file_stream.fail())
		throw std::ios_base::failure("failed to open file: " + this->file_name);
	std::cout << "fd " << this->file_fd << " is created as " << this->file_name << std::endl; //DEBUG
	std::cout << "Is file open ?: " << this->file_stream.is_open() << std::endl;
}


/* ************************************************************************** */
/*                             NO MEMBER FUNCTION                             */
/* ************************************************************************** */
