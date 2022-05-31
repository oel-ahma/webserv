#ifndef HEADER_HPP
# define HEADER_HPP

#define ERROR	-1
#define BACKLOG	1024
#define BUFF	1024

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
#include <poll.h>
#include <cerrno>

//C fcts
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct s_listen
{
	size_t port;
	std::string host;
}	t_listen;


bool isNumber(const std::string& s);

template<typename T>
void print_vector(std::vector<T> const &vec)
{
for (auto i = vec.begin(); i != vec.end(); ++i)
    std::cout << *i << "\n";
}

template<typename K, typename V> 
void print_map(std::map<K, V> const &m)
{
    for (auto it = m.begin(); it != m.end(); ++it) { //Carefull auto is C++11
        std::cout << (*it).first << ": " << (*it).second << "\n";
    }
}

#endif
