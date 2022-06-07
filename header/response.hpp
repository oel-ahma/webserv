#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "header.hpp"
#include "request.hpp"

class Request;

class Response {
	public:
		Response();
		Response(Response const &cpy);

		virtual ~Response();

		Response &operator=(Response const &other);

		void	prepareResponse(Request *request, std::map<int, std::string> errorPages);
		void	CreateTmpFile();
		void	createResponse(Request *request, std::map<int, std::string> errorPages);
		void	initStatusCodeMsg();
		
		//Setters
		void	setRoot(std::string root);
		void	setIndex(std::string index);

		//Http Methods
		int		httpGetMethod();
		int		httpPostMethod();
		int		httpDeleteMethod();
		int		setHeaders();


		char	buff[BUFF+1];
		int		sizeBuff;
		bool	responseIsSet;
		std::string		file_name;
		int 			file_fd;
		std::fstream	file_stream;

	private:
		Request								*_request;
		std::map<size_t, std::string>		_statusMsg;
		std::string							_statusLine;
		std::string							_headers;
		std::string							_body;
		std::string							_response;
		
		std::string							_root;
		std::string							_index;
};

#endif
