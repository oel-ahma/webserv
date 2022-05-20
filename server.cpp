#include "server.hpp"

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Server::Server() : addrlen(sizeof(address)), PORT(8080) {
}

Server::Server(Server const &other) : addrlen(sizeof(address)), PORT(8080) {
	*this = other;
}

/* ************************************************************************** */
/*                                 DESTRUCTOR                                 */
/* ************************************************************************** */

Server::~Server() {}

//* ************************************************************************** */
/*                                 OPERATORS                                  */
/* ************************************************************************** */

Server & Server::operator=(Server const &other) {
	this->socket_server = other.socket_server;
	this->address = other.address;
	this->addrlen = other.addrlen;
	this->PORT = other.PORT;
	this->fds = other.fds;
	return (*this);
}

/* ************************************************************************** */
/*                                 ACCESSORS                                  */
/* ************************************************************************** */

int	Server::get_port() const {
	return (this->PORT);
}

int		Server::get_socket_server() const {
	return(this->socket_server);
}

struct sockaddr_in*		Server::get_addr() {
	return(&this->address);
}

socklen_t*		Server::get_addrlen() const {
	return((socklen_t*)&this->addrlen);
}

/* ************************************************************************** */
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */

bool	Server::set_server() {
	if (this->set_socket() == false || this->bind_server() == false
		|| this->listen_server() == false)
		return(false);
	return (true);
}

bool	Server::set_socket() {
	//creer le socket du server et le stocke dans un int
	if ((this->socket_server = socket(PF_INET, SOCK_STREAM, 0)) == ERROR) {
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return(false);
	}
	//Ajoute des options au socket
	int optval = 1;
	if (setsockopt(this->socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == ERROR) {
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return(false);
	}
	return (true);
}

bool	Server::bind_server() {
	//set la structure sockaddr_in pour bind le server
	memset((char *)&this->address, 0, sizeof(this->address));
	this->address.sin_family = PF_INET;
	this->address.sin_port = htons(this->PORT);
	this->address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_server, (struct sockaddr *)&this->address, sizeof(this->address)) == ERROR) {
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return(false);
	}
	return (true);
}

bool	Server::listen_server() {
	if (listen(this->socket_server, BACKLOG) == ERROR) {
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return (false);
	}
	//Creer une structure pollfd pour y ajouter le socket du server
	struct pollfd fd;
	fd.fd = socket_server;
	fd.events = POLLIN;//TODO: not sur
	//Ajout du socket_server comme premier element du vector
	fds.push_back(fd);
	return (true);
}

/*
void	Server::routine() {
	//TODO: check sur fds[0] si une connection se fait sur le server: càd nouveau client
	//TODO: iterer sur le vector pour chaque socket
	//TODO: exectuer une action par socket seulement pour ne pas étre bloqué
}

//TODO:Faire une boucle avec toute ses fonctions dans routine.
//TODO:Une action par tour de boucle pour chaque fd.
long Server::accept()
{
    long socket = ::accept(socket_server, NULL, NULL);

    if (socket < 0)
        std::cerr << "Error: " << strerror(errno) << std::endl; // Still thing to add to this method in case of connection > 0
    return socket;
}

void Server::read(int socket)
{
  char buffer[100];
  long bytesRead = ::read(socket, buffer, 100); // change read with recv
  std::cout << "The message was: " << buffer;
}

void Server::send(int socket)
{
  // Send a message to the connection
  std::string response = "Good talking to you\n";
  ::send(socket, response.c_str(), response.size(), 0);
}
*/

// Close the connections TODO: fct cllose() autorisée ??? Elle n'est pas mentionné dans le sujet, dans les fonctions externe autorisée
void Server::close(int socket)
{
    ::close(socket);
    ::close(socket_server);
}
//TODO:lorsqu'un client se connecte au server, il faut set la structure pollfd pour ensuite l'ajouter au vector
void	Server::add_client() {//TODO:
	//creer une structure pollfd pour y ajouter le socket client
	struct pollfd fd;

	fd.fd = socket_server;
	fd.events = POLLIN;
	//Ajout du socket-client au vector
	fds.push_back(fd);
}
