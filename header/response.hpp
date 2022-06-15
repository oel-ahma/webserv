#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "header.hpp"
#include "request.hpp"

class Request;
class ConfigParse;

class Response {
	public:
		Response();
		Response(Response const &cpy);

		virtual ~Response();

		Response &operator=(Response const &other);

		void	fillBuffer();
		void	prepareResponse(Request *request, ConfigParse const *config);
		void	CreateTmpFile();
		void	createResponse();
		// void	setResponseLocation();
		void	initStatusCodeMsg();
		
		//Setters

		//Http Methods
		int		httpGetMethod();
		int		httpPostMethod();
		int		httpDeleteMethod();
		int		responseBodyErrorSet();
		int		setHeaders();


		char	buff[BUFF + 1];
		int		sizeBuff;
		bool	responseIsSet;
		std::string		file_name;
		int 			file_fd;
		std::fstream	file_stream;

	private:
		Request								*_request;
		ConfigParse const					*_config;
		std::map<size_t, std::string>		_statusMsg;
		std::string							_statusLine;
		std::string							_path;
		std::string							_headers;
		std::string							_body;
		std::string							_response;
		ConfigParse	const					*_responseLocation;

};

#endif
