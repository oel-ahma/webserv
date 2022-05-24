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
		int							socket_server; //const ?
		uint32_t					PORT;//TODO: le port restera toujours le même ? const ?
		std::vector<struct pollfd>	fds;
		//TODO: buffer pour stocket les send()/recv ?
		//TODO: iterator pour iterer sur le tab de buffer ?

		Server();
		void	set_socket();
		void	bind_server();
		void	listen_server();
		void	add_client();
		void	recv(int socket);
		void	send(int socket);
		// void	close(int socket);
};

#endif
