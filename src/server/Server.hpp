# pragma once


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
#define PORT 4445

class Config;

class Server
{
private:
    int _epollInstance;
    int _ServerFd;
    Config &_data;
    struct sockaddr_in _address;
    std::map<int, int> _ClientsFds;
    epoll_event _clients[MAX_CLIENTS_EVENTS];

public:
    int readyClients;
    Server(Config &data);
    int getServerFd();
    void addClientInEppol();
    void readClientRequest(epoll_event client);
    void sendHttpResponse(int clientFd);
    void CreateEpollInstance();
    void run();
    std::map<int, int> getClients() const;
    ~Server();
};

void throwing(std::string fct);
