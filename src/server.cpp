#include "server.hpp"

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Server::Server(Config const &conf) {
	(void)conf;
	//TODO:
}

Server::Server(Server const &other) {
	*this = other;
}

Server::Server() {}

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
		throw std::runtime_error(strerror(errno));
	//Ajoute des options au socket
	int optval = 1;
	if (setsockopt(this->socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == ERROR)
		throw std::runtime_error(strerror(errno));
}

void	Server::bind_server() {
	//set la structure sockaddr_in pour bind le server
	struct sockaddr_in addrServer;

	memset((char *)&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = PF_INET;
	addrServer.sin_port = htons(8080);
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_server, (struct sockaddr *)&addrServer, (socklen_t)sizeof(addrServer)) == ERROR)
		throw std::runtime_error(strerror(errno));
}

void	Server::listen_server() {
	if (listen(this->socket_server, BACKLOG) == ERROR)
		throw std::runtime_error(strerror(errno));
	//Creer une structure pollfd pour y ajouter le socket du server
	struct pollfd fd;
	fd.fd = socket_server;
	fd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	//Ajout du socket_server comme premier element du vector
	fds.push_back(fd);
}

// Lorsqu'un client se connecte au server, il faut set la structure pollfd pour ensuite l'ajouter au vector
void	Server::add_client() {
	struct pollfd pfd;
	struct sockaddr_in addrClient;

	if ((pfd.fd = ::accept(this->socket_server, NULL, NULL)) == ERROR)
		throw std::runtime_error(strerror(errno));
	pfd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	fds.push_back(pfd);
	//TODO: creer un fichier temporaire qui est stocké dans la class request du client
	//Message pour annoncer une nouvelle connexion client ?
}

void Server::recv(int socket) {
	this->client_buff[socket].sizeBuff = ::recv(socket, this->client_buff[socket].buff, BUFF, 0);
	if (this->client_buff[socket].sizeBuff == ERROR)
		throw std::runtime_error(strerror(errno));
	// this->client_buff[socket].buff[?] = '\0'; TODO: Mettre \0 a la fin du buff, ou alors clear le buffer a chaque fois


	// std::cout << "The message was:\n" << client_buff[socket].buff << std::endl; //DEGUB
}

bool Server::send(int socket) { //TODO: Send a message to the connection
	//TODO: autre condition.
	//	_Lorsque read renvoi EOF ?
	//	_Lorsque le TT de char lu < sizeBuff ?
	if (!this->response_buff[socket].empty())
	{
		// std::string response = "HTTP/1.1 201 OK\r\nDate: Sun, 18 Oct 2012 10:36:20 GMT\r\n\r\n<!DOCTYPE html><html><head><title>Example</title></head><body><p>This is an example of a simple HTML page with one paragraph.</p></body></html>\r\n";
		if (::send(socket, this->response_buff[socket].c_str(), this->response_buff[socket].size(), 0) == ERROR)
			throw std::runtime_error(strerror(errno));
		//std::cout << "Response Sent...\n";
		return (false);
	}
	return (true);
}

void	Server::treat_Request(int client) {
	//TODO: write le buffer dans un  fichier temporaire puis clear le buffer
	this->response_buff[client] = "HTTP/1.1 201 OK\r\nDate: Sun, 18 Oct 2012 10:36:20 GMT\r\n\r\n<!DOCTYPE html><html><head><title>Example</title></head><body><p>This is an example of a simple HTML page with one paragraph.</p></body></html>\r\n";
}

void	Server::routine() {
	if (poll(&fds[0], fds.size(), 0) != ERROR) {
		if (fds[0].revents & POLLIN)
			add_client();
		for (std::vector<struct pollfd>::iterator it = this->fds.begin() + 1; it != this->fds.end(); it++) {
			if (!this->client_buff[it->fd].sizeBuff)
				treat_Request(it->fd);
			else if (it->revents & POLLIN) {
				this->recv(it->fd);
				std::cout << it->revents << POLLIN << POLLOUT << std::endl;
			}
			else if (it->revents & POLLOUT) {
				if (this->send(it->fd) == true) {
					//quand le la reponse est envoyée, fermer le socket
					close(it->fd);
					continue;
				}
				this->send(it->fd);
			}
			else if (it->revents & POLLRDHUP || it->revents & POLLERR) {
				close(it->fd);
			}
		}
	}
	else
		throw std::runtime_error(strerror(errno));
}

void Server::close(int socket) {
	this->client_buff.erase(socket);
	this->response_buff.erase(socket);
	::close(socket);
}
