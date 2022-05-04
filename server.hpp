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

class Server
{
    public:
        Server();
        Server(Server &other);
        ~Server();

        Server &operator=(Server const &other);

        int     setup_socket();
        long    accept();
        void    read(int socket);
        void    send(int socket);
        void    close(int socket);

    private:
        int _sockfd;
        sockaddr_in _sockaddr;
};