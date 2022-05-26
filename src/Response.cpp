// #include "Response.hpp"
#include "../header/Response.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Response::Response() : sizeBuff(0), responseIsSet(false) {
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
	//Parser la requete pour creer une reponse dans un tmpFile
	//remplir le buffer avec read()
	// this->buff = "HTTP/1.1 201 OK\r\nDate: Sun, 18 Oct 2012 10:36:20 GMT\r\n\r\n<!DOCTYPE html><html><head><title>Example</title></head><body><p>This is an example of a simple HTML page with one paragraph.</p></body></html>\r\n";
	this->responseIsSet = true;
}


/* ************************************************************************** */
/*                             NO MEMBER FUNCTION                             */
/* ************************************************************************** */
