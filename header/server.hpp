#ifndef _SERVER_HPP
# define _SERVER_HPP

#include "header.hpp"
#include "config.hpp"
#include "request.hpp"
#include "response.hpp"

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
		int							socket_server;//TODO: const ?
		uint32_t					PORT = 8080;//TODO: const ?
		std::string					root = "/www";
		std::vector<struct pollfd>	fds;
		std::map<int, Request>		client_buff;
		std::map<int, Response>		response_buff;	//TODO: Class response ? mettre le buffer CHAR dans la class response

		void	set_socket();
		void	bind_server();
		void	listen_server();
		void	add_client();
		void	treat_Request(int client);
		void	recv(int socket);
		bool	send(int socket);
		void	close(std::vector<struct pollfd>::iterator it);
		bool	ready;
};

#endif