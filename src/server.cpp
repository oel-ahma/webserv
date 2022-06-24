// #include "server.hpp"
#include "../header/server.hpp" //TODO:

/* ************************************************************************** */
/*                                CONSTRUCTORS                                */
/* ************************************************************************** */

Server::Server(ConfigParse const &conf) : ready(false), config(&conf) {}

Server::Server(Server const &other) {
	*this = other;
}

Server::Server() : ready(false) {}

/* ************************************************************************** */
/*                                 DESTRUCTOR                                 */
/* ************************************************************************** */

Server::~Server() {}

//* ************************************************************************** */
/*                                 OPERATORS                                  */
/* ************************************************************************** */

Server & Server::operator=(Server const &other) {
	this->socket_server = other.socket_server;
	this->fds = other.fds;
	this->ready = other.ready;
	this->config = other.config;
	this->client_buff = other.client_buff;
	this->response_buff = other.response_buff;
	return (*this);
}

/* ************************************************************************** */
/*                                 ACCESSORS                                  */
/* ************************************************************************** */

int		Server::get_socket_server() const {
	return(this->socket_server);
}

bool Server::getReady() {
	return this->ready;
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
		throw std::runtime_error("socket(): " + (std::string)strerror(errno));
	//Ajoute des options au socket
	fcntl(this->socket_server, F_SETFL, O_NONBLOCK);
	int optval = 1;
	if (setsockopt(this->socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == ERROR)
		throw std::runtime_error("setsockopt(): " + (std::string)strerror(errno));
}

void	Server::bind_server() {
	//set la structure sockaddr_in pour bind le server
	struct sockaddr_in addrServer;

	memset(&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = PF_INET;
	addrServer.sin_port = htons(this->config->getListen().port);
	addrServer.sin_addr.s_addr = inet_addr(this->config->getListen().host.c_str());
	if (bind(this->socket_server, (struct sockaddr *)&addrServer, (socklen_t)sizeof(addrServer)) == ERROR)
		throw std::runtime_error("bind(): " + std::string(strerror(errno)));
}

void	Server::listen_server() {
	if (listen(this->socket_server, BACKLOG) == ERROR)
		throw std::runtime_error("listen(): " + std::string(strerror(errno)));
	//Creer une structure pollfd pour y ajouter le socket du server
	struct pollfd fd;
	fd.fd = socket_server;
	fd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	//Ajout du socket_server comme premier element du vector
	fds.push_back(fd);
	this->ready = true;
	std::cout << "Server up. Socket: " << fds[0].fd << std::endl;
}

// Lorsqu'un client se connecte au server, il faut set la structure pollfd pour ensuite l'ajouter au vector
void	Server::add_client() {
	struct pollfd pfd;
	struct sockaddr_in addrClient;
	socklen_t size = sizeof(addrClient);
	memset(&addrClient, 0, sizeof(addrClient));
	if ((pfd.fd = ::accept(this->socket_server, (struct sockaddr *)&addrClient, &size)) == ERROR)
		throw std::runtime_error("accept(): " + (std::string)strerror(errno));
	pfd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	fds.push_back(pfd);
	this->client_buff[pfd.fd].ClientSocket = pfd.fd;
	std::cout << "Client " << pfd.fd << " connected.\n" << std::endl;
}

bool Server::recv(int socket) {
	char buff[BUFF + 1];
	int rd;

	rd = ::recv(socket, buff, BUFF, 0);
	buff[rd] = '\0';
	this->client_buff[socket].setRequest(buff, rd);
	memset(buff, 0, BUFF);
	// std::cout << RED << "we have received: " << rd   << RESET << std::endl;
	// std::cout << "recv for the client " << socket << ".\n" << this->client_buff[socket].sizeBuff <<std::endl;
	if (this->client_buff[socket].sizeBuff == ERROR)
	{
		std::cout << "ERROR" << std::endl;
		this->client_buff[socket].setStatusCode(500);
		response_buff[socket].prepareResponse(&client_buff[socket], this->config);
		return true;
	}
	else if (rd == 0 && this->client_buff[socket].getRequest().empty())
		return false;
	std::cout << GREEN << "The message was:" << socket  << "\n" << this->client_buff[socket].getRequest() << this->client_buff[socket].getStatusCode() << std::endl << "response is: \n" <<  this->response_buff[socket].getResponse() << RESET << std::endl; //DEGUB
	return true;
}

bool Server::send(int socket) {
	if (this->response_buff[socket].responseIsSet == false && !this->client_buff[socket].getRequest().empty()) {
		client_buff[socket].parsing();
		response_buff[socket].prepareResponse(&client_buff[socket], this->config);
	}
	else if (this->response_buff[socket].responseIsSet == true && !this->client_buff[socket].getRequest().empty()) { //TODO: else or if !!!!!
		// std::cout << "We are sending this message:" << "\n" << this->response_buff[socket].getResponse() << "\n" << this->response_buff[socket].getResponse().size() << std::endl;
		if (::send(socket, this->response_buff[socket].getResponse().c_str(), this->response_buff[socket].getResponse().size(), 0) == ERROR)
			return false;
		return (true);
	}
	return (false);
}

void	Server::routine() {
	if (poll(&fds[0], fds.size(), TIMEOUT) != ERROR) {
		if (this->ready == false)
		{
			this->listen_server();
			return;	
		}	
		else if (fds[0].revents & POLLIN) {
			add_client();
			return ;
		}
		for (std::vector<struct pollfd>::iterator it = this->fds.begin(), ite = this->fds.end(); it != ite; ++it) {
			if (it->revents & POLLIN) {
				std::cout << "socket " << it->fd << " ----POLLIN---------" << std::endl;
				if (this->recv(it->fd) == false)
					close(it);
				// it->events = POLLOUT;
			}
			else if (it->revents & POLLOUT) {
				std::cout << "socket " << it->fd << " ----POLLOUT---------" << std::endl;
				if (this->send(it->fd) == true) {
					close(it);
					continue;
				}
				// it->events = POLLIN;
			}
			else if (it->revents & POLLRDHUP || it->revents & POLLERR) {
				std::cout << "socket " << it->fd << " ----close dans POLLRDHUP ou POLLERR---------" << std::endl;
				close(it);
			}
		}
	}
	else {
		// std::cout << "------------error poll------------" << std::endl;
		throw std::runtime_error("poll(): " + std::string(strerror(errno)));
	}
}

void Server::close(std::vector<struct pollfd>::iterator it) {
	this->client_buff.erase(it->fd);
	this->response_buff.erase(it->fd);
	this->fds.erase(it);
	::close(it->fd);
	std::cout << "Client " << it->fd << " disconnected" << std::endl;
}
