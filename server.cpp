#include "server.hpp"

int Server::setup()
{ 
    _sockfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET : Internet Protocol v4 addresses ; SOCK_STREAM : TCP not UDP
    if (_sockfd == -1) 
    {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    memset(&_sockaddr, 0, sizeof(sockaddr)); // need to check the first memset param
    _sockaddr.sin_family = AF_INET;
    _sockaddr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY need to be changed to a more general variable 
    _sockaddr.sin_port = htons(9898); // Same here for 9898 and need to code htons
    
    if (bind(s, (struct sockaddr*)&_sockaddr, sizeof(sockaddr)) < 0) 
    {
        std::cerr << "Failed to bind to port 9898." << std::endl;
        return -1;
    }

    // Start listening. Hold at most 1000 connections in the queue
    if (listen(_sockfd, 1000) < 0) 
    {
        std::cerr << "Failed to listen on socket." << std::endl;
        return -1;
    }
    return 0;
}

long Server::accept()
{
    long socket = ::accept(_sockfd, NULL, NULL);

    if (socket < 0)
        std::cerr << "Failed to grab connection." << std::endl; // Still thing to add to this method in case of connection > 0
    return socket;
}

void Server::read(int socket)
{
  char buffer[100];
  long bytesRead = ::read(socket, buffer, 100); // change read with recv
  std::cout << "The message was: " << buffer;
}

void Server::send(int socket)
{
  // Send a message to the connection
  std::string response = "Good talking to you\n";
  ::send(socket, response.c_str(), response.size(), 0);
}

// Close the connections
void Server::close(int socket)
{
    ::close(socket);
    ::close(_sockfd);
}

// Constructors and Destructor

Server::Server()
{
}

Server::Server(Server &other)
{
    *this = other;
}

Server::~Server()
{
}

// operator= overload

Server & Server::operator=(Server const &other)
{
    _sockfd = other._sockfd;
    _sockaddr = other._sockaddr;
    return (*this); 
}

int Server::getSockFd() { return _sockfd; }
