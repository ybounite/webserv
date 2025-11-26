#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdexcept>
#include <fcntl.h>
#include <poll.h>
#include <sys/epoll.h>
#include <vector>
#include <map>

# include "../../includes/Webserv.hpp"

#define PORT 8080
#define MAXREAD 4086

typedef struct {
    int id;
    std::string inBuffer;
    std::string outBuffer;
}   t_clients;

#define MAX_CLIENTS_EVENTS 10

class Socket
{
private:
    int _epollInstance;
    int _SockeFd;
    struct sockaddr_in _address;
    std::map<int, t_clients> _ClientsMap;
    

public:
    Socket();
    // Socket(Config	&config );
    int getSocketFd();
    void addClientInEppol();
    void readClientRequest(epoll_event client);
    void sendHttpResponse(int clientFd);
    void run();
    std::map<int, t_clients> getClients() const;
    ~Socket();
};

#endif