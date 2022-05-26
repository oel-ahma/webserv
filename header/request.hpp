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
        std::string                          _query;
        std::string                         _path;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _statusCode;
        std::string                         _rawString;
        std::vector<std::string>            _methodList;

    public:
        Request();
        Request(std::string const &str);
        Request(Request const &other);
        ~Request();

        Request &operator=(Request const &rhs);

        void        initHeaders();
        void        initMethodList();
        void        parsing(std::string const &str);
        std::string gnl(std::string const &str, size_t &i);
        void        parseFirstLine(std::string const &str);
		void		CreatetTmpFile();

		char								buff[BUFF+1];
		int									sizeBuff;
		int									tmpFile;
		std::string							fileName;
		std::fstream						file;

        //Getters
        std::map<std::string, std::string>  getHeaders();
        std::string                         getBody();
        size_t                              getStatusCode();
        std::string                         getRawString();



        //Debug Methods
        void printHeaders();
};


template<typename K, typename V> 
void print_map(std::map<K, V> const &m)
{
    for (auto it = m.begin(); it != m.end(); ++it) { //Carefull auto is C++11
        std::cout << (*it).first << ": " << (*it).second << "\n";
    }
}

#endif
