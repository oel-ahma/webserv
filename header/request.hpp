// Receive a String from the Server Process Program 
// >> Parse it 
// >> Set All the methods and Get header fields values.

#ifndef _REQUEST_HPP
# define _REQUEST_HPP

#include "header.hpp"

class Request
{
    private:
        std::string                         _method;
        std::string                         _query;
        std::string                         _path;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _statusCode;
        std::string                         _rawString;
        std::vector<std::string>            _methodList;
		std::string							_request;
		//Not Sure If It has to be in this Class	

    public:
        Request();
        Request(std::string const &str);
        Request(Request const &other);
        ~Request();

        Request &operator=(Request const &rhs);

        void        initHeaders();
        void        initMethodList();
        void        parsing();
		void		clearAll();
        std::string gnl(std::string const &str, size_t &i);
        void        parseFirstLine(std::string const &str);
		
		char								buff[BUFF+1];
		int									sizeBuff;

        //Getters
        std::map<std::string, std::string>  getHeaders() const;
        std::string                         getBody() const;
        size_t                              getStatusCode() const;
        std::string                         getRawString() const;
		std::string							getMethod() const;
		std::string							getQuery() const;
		std::string							getPath() const;
		std::string							getVersion() const;
		std::string							getRequest() const;
        //Setters
		void								setStatusCode(size_t statusCode);
		void								setVersion(std::string const &str);
		void								setRequest(char *buff, int size);
		//Added Members 30/05
		int									ClientSocket;

};

#endif
