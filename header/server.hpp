#ifndef _SERVER_HPP
# define _SERVER_HPP

#include "header.hpp"

class ConfigParse;
class Response;
class Request;
class ConfigParse;

class Server
{
	public:
		Server();
		Server(ConfigParse const &conf);
		Server(Server const &other);
		~Server();

		Server &operator=(Server const &other);

		void	set_server();
		void	routine();

		//GETTERS
		int						get_socket_server() const;
		bool					getReady();


	private:
		int							socket_server;
		std::vector<struct pollfd>	fds;
		std::map<int, Request>		client_buff;
		std::map<int, Response>		response_buff;
		bool						ready;
		const ConfigParse			*config;

		void	set_socket();
		void	bind_server();
		void	listen_server();
		void	add_client();
		void	treat_Request(int client);
		bool	recv(int socket);
		bool	send(int socket);
		void	close(std::vector<struct pollfd>::iterator it);
};

#endif