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

		void	prepareResponse(Request *request, ConfigParse const *config);
		void	createResponse();
		// void	setResponseLocation();
		void	initStatusCodeMsg();
		
		std::string getResponse();
		//Setters

		//Http Methods
		int		httpGetMethod();
		int		httpPostMethod();
		int		httpDeleteMethod();
		int		responseBodyErrorSet();
		int		responseBodyDirectorySet();
		int		setHeaders();


		char	buff[BUFF + 1];
		int		sizeBuff;
		bool	responseIsSet;

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
