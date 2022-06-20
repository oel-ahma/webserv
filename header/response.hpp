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
		void	initContentTypeMIME();
		void	initStatusCodeMsg();
		void	clearAll();
		std::string getResponse();
		//Setters

		//Http Methods
		int		httpGetMethod();
		int		httpPostMethod();
		int		httpDeleteMethod();
		void	execCGI();
		void	setEnvCGI();
		int		responseBodyErrorSet();
		int		responseBodyDirectorySet(size_t flag);
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
		std::string							_contentTypeCgi;
		ConfigParse	const					*_responseLocation;
		std::map<std::string, std::string>  _contentTypeMIME;
		sockaddr_in							_addr;

};

#endif
