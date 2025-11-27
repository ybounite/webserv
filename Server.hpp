#ifndef SERVER_HPP
#define SERVER_HPP

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

#define MAX_CLIENTS_EVENTS 1000

class Server
{
private:
    int _epollInstance;
    int _ServerFd;
    struct sockaddr_in _address;
    std::vector<int> _ClientsFds;
    epoll_event _clients[MAX_CLIENTS_EVENTS];

public:
    int readyClients;
    Server();
    int getServerFd();
    void addClientInEppol();
    void readClientRequest(epoll_event client);
    void sendHttpResponse(int clientFd);
    void CreateEpollInstance();
    void run();
    std::vector<int> getClients() const;
    ~Server();
};

void throwing(std::string fct);

#endif