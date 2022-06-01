#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "header.hpp"



class Response {
	public:
		Response();
		Response(Response const &cpy);

		virtual ~Response();

		Response &operator=(Response const &other);

		void	prepareResponse(Request *request);
		void	CreateTmpFile();
		void	createResponse(Request *request);
		void	initStatusCodeMsg();
		
		//Setters
		void	setRequest(Request *other);
		
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

		Request			_request;
	private:
		std::map<size_t, std::string>	_statusMsg;
		std::string							_statusLine;
		std::string							_headers;
		std::string							_body;
		std::string							_response;
};

#endif
