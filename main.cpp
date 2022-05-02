#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include "server.hpp"


int main() 
{
  Server serv;

  if (serv.setup_socket() == 0)
  {
    long socket = serv.accept();
    serv.read(socket);
    serv.send(socket);
    serv.close(socket);
  }
  return 0;
}