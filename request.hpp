// Receive a String from the Server Process Program 
// >> Parse it 
// >> Set All the methods and Get header fields values.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

class Request
{
    private:
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _statusCode;
        std::string                         _rawString;

    public:
        Request();
        Request(std::string const &str);
        Request(Request const &other);
        ~Request();

        Request &operator=(Request const &rhs);

        void initHeaders();
        void parsing(std::string const &str);

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
    for (auto it = m.begin(); it != m.end(); ++it) {
        std::cout << (*it).first << ": " << (*it).second << "\n";
    }
}