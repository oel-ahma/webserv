#include "server.hpp"

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Server::Server(Config const &conf) {
	//TODO:
}

Server::Server(Server const &other) {
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

/* ************************************************************************** */
/*                              MEMBER FUNCTION                               */
/* ************************************************************************** */

void	Server::set_server() {
	this->set_socket();
	this->bind_server();
	this->listen_server();
	//Message pour annoncer la creation du socket server ?
}

void	Server::set_socket() {
	//creer le socket du server et le stocke dans un int
	if ((this->socket_server = socket(PF_INET, SOCK_STREAM, 0)) == ERROR)
		throw strerror(errno);
	//Ajoute des options au socket
	int optval = 1;
	if (setsockopt(this->socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == ERROR)
		throw strerror(errno);
}

void	Server::bind_server() {
	//set la structure sockaddr_in pour bind le server
	struct sockaddr_in addrServer;

	memset((char *)&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = PF_INET;
	addrServer.sin_port = htons(this->PORT);
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_server, (struct sockaddr *)&addrServer, (socklen_t)sizeof(addrServer)) == ERROR)
		throw strerror(errno);
}

void	Server::listen_server() {
	if (listen(this->socket_server, BACKLOG) == ERROR)
		throw strerror(errno);
	//Creer une structure pollfd pour y ajouter le socket du server
	struct pollfd fd;
	fd.fd = socket_server;
	fd.events = POLLIN;
	//Ajout du socket_server comme premier element du vector
	fds.push_back(fd);
}

// Lorsqu'un client se connecte au server, il faut set la structure pollfd pour ensuite l'ajouter au vector
void	Server::add_client() {
	struct pollfd pfd;
	struct sockaddr_in addrClient;

	if ((pfd.fd = ::accept(this->socket_server, (struct sockaddr *)&addrClient, (socklen_t*)sizeof(addrClient))) == ERROR)
		throw strerror(errno);
	pfd.events = POLLIN;
	fds.push_back(pfd);
	//Message pour annoncer une nouvelle connexion client ?
}

void Server::recv(int socket) { //TODO:
	//TODO: buffer dans la classe server
	char buffer[100];

	if (long bytesRead = ::recv(socket, buffer, 100, 0) == ERROR)
		throw strerror(errno);
	std::cout << "The message was: " << buffer;
}

void Server::send(int socket) { //TODO:
	// Send a message to the connection
	std::string response = "Good talking to you\n";
	if (::send(socket, response.c_str(), response.size(), 0) == ERROR)
		throw strerror(errno);
}

void	Server::routine() {
	// exectuer une action par socket seulement pour ne pas étre bloqué
	if (poll(&fds[0], fds.size(), 0) != ERROR) {
		// check sur fds[0] si une connection se fait sur le server: càd nouveau client
		if (fds[0].revents & POLLIN)
			add_client();
		// On itère sur chaque socket
		for (std::vector<struct pollfd>::iterator it = this->fds.begin() + 1; it != this->fds.end(); it++) {
			//TODO: il faut soit recevoir la requete, soit executer une requete faite au prealable
			//TODO: si le buffer n'est pas vide, alors une commande est a executer
			//...
			// Client qui envoi une requete
			if (it->revents & POLLIN) {
				this->send(it->fd);
			}
			// Client qui attend une réponse à sa requete
			else if (it->revents & POLLOUT) {
				this->recv(it->fd);
			}
			//TODO: Deco client ?
		}
	}
	else
		throw strerror(errno);
}

/*
// Close the connections TODO: fct close() autorisée ??? Elle n'est pas mentionné dans le sujet, dans les fonctions externe autorisée
void Server::close(int socket)
{
    ::close(socket);
    ::close(socket_server);
}
*/
