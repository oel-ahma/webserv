// #include "server.hpp"
#include "../header/server.hpp" //TODO:

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
	this->ready = true;
	std::cout << "Server up." << std::endl;
}

// Lorsqu'un client se connecte au server, il faut set la structure pollfd pour ensuite l'ajouter au vector
void	Server::add_client() {
	struct pollfd pfd;
	struct sockaddr_in addrClient;
	socklen_t size = sizeof(addrClient);

	if ((pfd.fd = ::accept(this->socket_server, (struct sockaddr *)&addrClient, &size)) == ERROR)
		throw std::runtime_error(strerror(errno));
	pfd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	fds.push_back(pfd);
	this->client_buff[pfd.fd].CreatetTmpFile();
	std::cout << "Client " << pfd.fd << " connected.\n" << this->fds.size() << std::endl;
}

void Server::recv(int socket) {
	this->client_buff[socket].sizeBuff = ::recv(socket, this->client_buff[socket].buff, BUFF, 0);
	if (this->client_buff[socket].sizeBuff == ERROR)
		throw std::runtime_error(strerror(errno));
	this->client_buff[socket].buff[this->client_buff[socket].sizeBuff] = '\0';
	// std::cout << "The message was:\n" << client_buff[socket].buff << std::endl; //DEGUB
}

bool Server::send(int socket) {
	//preparer la reponse avant de l'envoyer
	if (this->response_buff[socket].responseIsSet == false)
		response_buff[socket].prepareResponse();
	else {
		if (::send(socket, this->response_buff[socket].buff, this->response_buff[socket].sizeBuff, 0) == ERROR)
			throw std::runtime_error(strerror(errno));
		this->response_buff[socket].responseIsSet = false;
		if (this->response_buff[socket].sizeBuff < BUFF)
			return (true);
	}
	return (false);
}

void	Server::treat_Request(int client) {
	//TODO: write le buffer dans un  fichier temporaire puis clear le buffer
}

void	Server::routine() {//listen poll
	if (poll(&fds[0], fds.size(), 0) != ERROR) {
		if (this->ready == false)
			this->listen_server();
		else if (fds[0].revents & POLLIN)
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
					close(it->fd);
					continue;
				}
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
	this->client_buff[socket].file.close();
	if (this->client_buff[socket].file.fail() || this->response_buff[socket].file.fail())
		throw std::ios_base::failure("failed to close file: " + this->client_buff[socket].fileName);
	this->response_buff[socket].file.close();
	if (this->response_buff[socket].file.fail())
		throw std::ios_base::failure("failed to close file: " + this->response_buff[socket].fileName);
	if (remove(this->client_buff[socket].fileName.c_str()) == ERROR
		|| remove(this->response_buff[socket].fileName.c_str()) == ERROR)
		throw std::runtime_error(strerror(errno));
	this->client_buff.erase(socket);
	this->response_buff.erase(socket);
	std::cout << "Client " << socket << " disconnected" << std::endl;
	::close(socket);
}
