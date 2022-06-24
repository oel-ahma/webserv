#ifndef HEADER_HPP
# define HEADER_HPP

#define ERROR	-1
#define BACKLOG	65535
#define BUFF	65535
#define TIMEOUT 100

#define RED		"\033[0;31m"
#define YELLOW	"\033[0;33m"
#define GREEN 	"\033[0;32m"
#define RESET	"\033[0;37m"

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
#include <poll.h>
#include <fcntl.h>
#include <cerrno>
#include <ctime>
#include <algorithm>


//C fcts
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

typedef struct	s_listen
{
	size_t port;
	std::string host;
}				t_listen;

#include "config.hpp"
#include "request.hpp"
#include "response.hpp"
#include "configParse.hpp"
#include "config.hpp"
#include "server.hpp"




bool 		isNumber(const std::string& s);
bool 		isFile(std::string const &path);
bool 		isDirectory(std::string const &path);
void 		removeDoubleSlash(std::string &str);
void		replacePercent20BySpaces(std::string &str);
int 		hostSyntaxCheck(std::string &str);

// template<typename T>
// void print_vector(std::vector<T> const &vec)
// {
// 	for (auto i = vec.begin(); i != vec.end(); ++i)
// 		std::cout << *i << "\n";
// }

// template<typename K, typename V> 
// void print_map(std::map<K, V> const &m)
// {
//     for (auto it = m.begin(); it != m.end(); ++it) { //TODO: Carefull auto is C++11
//         std::cout << YELLOW << "[" << (*it).first << "]: \n" << RESET << (*it).second << std::endl;
//     }
// }

#endif
