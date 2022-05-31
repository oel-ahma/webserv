#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "header.hpp"

class Response {
	public:
		Response();
		Response(Response const &cpy);

		virtual ~Response();

		Response &operator=(Response const &other);

		void	prepareResponse();
		void	CreateTmpFile();

		char	buff[BUFF+1];
		int		sizeBuff;
		bool	responseIsSet;

		std::string	file_name;
		int file_fd;
		std::fstream	file_stream;
	private:
};

#endif
