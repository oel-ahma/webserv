#ifndef _WEBSERV_HPP
# define _WEBSERV_HPP

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>


typedef struct s_listen
{
	size_t port;
	std::string host;
}	t_listen;


bool isNumber(const std::string& s);
template<typename T>
void print_vector(std::vector<T> const &vec);

#endif