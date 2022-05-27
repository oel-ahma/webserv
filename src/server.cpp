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
		throw std::runtime_error("socket(): " + (std::string)strerror(errno));
	//Ajoute des options au socket
	int optval = 1;
	if (setsockopt(this->socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == ERROR)
		throw std::runtime_error("setsockopt(): " + (std::string)strerror(errno));

}

void	Server::bind_server() {
	//set la structure sockaddr_in pour bind le server
	struct sockaddr_in addrServer;

	memset((char *)&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = PF_INET;
	addrServer.sin_port = htons(8080);
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_server, (struct sockaddr *)&addrServer, (socklen_t)sizeof(addrServer)) == ERROR)
		throw std::runtime_error("bind(): " + (std::string)strerror(errno));
}

void	Server::listen_server() {
	if (listen(this->socket_server, BACKLOG) == ERROR)
		throw std::runtime_error("listen(): " + (std::string)strerror(errno));
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
		throw std::runtime_error("accept(): " + (std::string)strerror(errno));
	pfd.events = POLLIN | POLLOUT | POLLRDHUP | POLLERR;
	fds.push_back(pfd);
	this->client_buff[pfd.fd].ClientSocket = pfd.fd;
	this->client_buff[pfd.fd].CreatetTmpFile();
	std::cout << "Client " << pfd.fd << " connected.\n" << std::endl;
}

void Server::recv(int socket) {
	this->client_buff[socket].sizeBuff = ::recv(socket, this->client_buff[socket].buff, BUFF, 0);
	if (this->client_buff[socket].sizeBuff == ERROR)
		throw std::runtime_error("recv(): " + (std::string)strerror(errno));
	this->client_buff[socket].buff[this->client_buff[socket].sizeBuff] = '\0';
	// std::cout << "The message was:\n" << client_buff[socket].buff << std::endl; //DEGUB
}

bool Server::send(int socket) {
	//preparer la reponse avant de l'envoyer
	if (this->response_buff[socket].responseIsSet == false) {
		std::cout << "client " << socket << " has now a response file." << std::endl;
		response_buff[socket].prepareResponse();
		std::cout << "file " << this->response_buff[socket].file_name << " is associated with client " << socket << ".\n" << std::endl;
	}
	else {
		std::cout << "We are sending this message:\n" << this->response_buff[socket].buff << "\n" << std::endl;
		if (::send(socket, this->response_buff[socket].buff, this->response_buff[socket].sizeBuff, 0) == ERROR)
			throw std::runtime_error("send(): " + (std::string)strerror(errno));
		this->response_buff[socket].responseIsSet = false;
		if (this->response_buff[socket].sizeBuff < BUFF)
			return (true);
	}
	return (false);
}

void	Server::treat_Request(int client) {
	//TODO: write le buffer dans le fichier temporaire du client_buff
	std::cout << "BUFFER de requete write dans le fichier tmp" << std::endl;
	this->client_buff[client].file_stream.write(this->client_buff[client].buff, this->client_buff[client].sizeBuff);
}

void	Server::routine() {//listen poll
	if (poll(&fds[0], fds.size(), 0) != ERROR) {
		if (this->ready == false)
			this->listen_server();
		else if (fds[0].revents & POLLIN) {
			add_client();
			return ;
		}
		for (std::vector<struct pollfd>::iterator it = this->fds.begin() + 1; it != this->fds.end(); it++) {
			if (this->client_buff[it->fd].sizeBuff)
				treat_Request(it->fd);
			else if (it->revents & POLLIN)
				this->recv(it->fd);
			else if (it->revents & POLLOUT) {
				std::cout << "socket " << it->fd << " ----POLLOUT---------" << std::endl;
				if (this->send(it->fd) == true) {
					std::cout << "socket " << it->fd << " ----close dans POLLOUT---------" << std::endl;
					close(it->fd);
					continue;
				}
			}
			else if (it->revents & POLLRDHUP || it->revents & POLLERR) {
				std::cout << "socket " << it->fd << " ----close dans POLLRDHUP ou POLLERR---------" << std::endl;
				close(it->fd);
			}
		}
	}
	else {
		std::cout << "------------error poll------------" << std::endl;
		throw std::runtime_error("poll(): " + (std::string)strerror(errno));
	}
}

//Verification d'erreur pas sûr. Nécessaire de vérif les close() ???
//google it: std::fstream
void Server::close(int socket) {
	std::cout << "---------------here1---------------" << std::endl;
	if (this->client_buff[socket].file_stream.is_open()) {
		this->client_buff[socket].file_stream.close();
		if (this->client_buff[socket].file_stream.fail())
			throw std::ios_base::failure("failed to close file: " + this->client_buff[socket].file_name);
		if (remove(this->client_buff[socket].file_name.c_str()) == ERROR)
			throw std::runtime_error("remove(): " + (std::string)strerror(errno));
	}
	std::cout << "---------------here2---------------" << std::endl;
	if (this->response_buff[socket].file_stream.is_open()) {
		this->response_buff[socket].file_stream.close();//TODO: le flag failbit est set, pourquoi ? car le fichier existe pourtant.
		if (this->response_buff[socket].file_stream.fail())
			throw std::ios_base::failure("failed to close file: " + this->response_buff[socket].file_name);
		if (remove(this->response_buff[socket].file_name.c_str()) == ERROR)
			throw std::runtime_error("remove(): " + (std::string)strerror(errno));
	}
	std::cout << "---------------here3---------------" << std::endl;
	this->client_buff.erase(socket);
	this->response_buff.erase(socket);
	std::cout << "Client " << socket << " disconnected" << std::endl;
	::close(socket);
}
