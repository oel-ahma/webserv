#include "server.hpp"

class Config
{
    public:
        Config();
        ~Config();
        Config(Config const &other);

        std::vector<std::string> readConfigFile(std::string const &fileName);
        int                      parseConfigFile(std::vector<std::string> const configFile);

        Config &operator=(Config const &other);
    private:
    std::string                 _listen; //it's not the right type of variable
    std::string                 _root;
    std::string                 _location; //it's not the right type of variable
    
    std::string                 _serverName;
    std::map<int, std::string>  _errorPages;
    long                        _clientMaxBodySize;
    std::vector<std::string>    _allowedMethods;
    std::vector<std::string>    _index;
    bool                        _autoindex;
    std::string                 _cgiPath; //it's not the right type of variable
    std::string                 _cgiExtension; //it's not the right type of variable
    std::string                 _alias;

};

template<typename T>
void print_vector(std::vector<T> const &vec)
{
for (auto i = vec.begin(); i != vec.end(); ++i)
    std::cout << *i << "\n";
}