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
        Server(Server const &other);
        ~Server();

        Server &operator=(Server const &other);

		bool	set_server();
		void	add_client();
		long	accept();
		void	read(int socket);
		void	send(int socket);
		void	close(int socket);
		void	routine();
		
		
		//GETTERS
		int						get_socket_server() const;
		int						get_port() const;
		struct sockaddr_in*		get_addr();
		socklen_t*				get_addrlen() const;


	private:
		int							socket_server;
		struct sockaddr_in			address;
		socklen_t					addrlen;
		uint32_t					PORT;//TODO: le port restera toujours le même ? const ?
		std::vector<struct pollfd>	fds;

		bool	set_socket();
		bool	bind_server();
		bool	listen_server();
};

#endif
