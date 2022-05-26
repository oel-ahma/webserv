#ifndef _SERVER_HPP
# define _SERVER_HPP

#define ERROR -1
#define BACKLOG 1000 //TODO: definir une taille logique

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <poll.h>
#include <cerrno>

//C fcts
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "config.hpp"
#include "request.hpp"

class Server
{
	public:
		Server();
		Server(Config const &conf);
		Server(Server const &other);
		~Server();

		Server &operator=(Server const &other);

		void	set_server();
		void	routine();

		//GETTERS
		int						get_socket_server() const;
		int						get_port() const;

	private:

		struct sockaddr_in addrClient;

		int							socket_server;//TODO: const ?
		uint32_t					PORT = 8080;//TODO: const ?
		std::vector<struct pollfd>	fds;
		std::map<int, Request>		client_buff;
		std::map<int, std::string>	response_buff;	//TODO: Class response ? mettre le buffer CHAR dans la class response

		void	set_socket();
		void	bind_server();
		void	listen_server();
		void	add_client();
		void	treat_Request(int client);
		void	recv(int socket);
		bool	send(int socket);
		void	close(int socket);
};

#endif
