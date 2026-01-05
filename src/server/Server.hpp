#pragma once

#include "../../includes/Webserv.hpp"

class Response;

#define MAX_CLIENTS_EVENTS 1000

class Config;

typedef struct s_clients
{
    s_clients() : fd(-1), firstTime(true), CGIfd(-1), leftData(false), bytesread(0), clsResponse(NULL) {}
    int fd;
    std::string request;
    std::string response;
    bool firstTime;
    int CGIfd;
    bool leftData;
    ssize_t bytesread;
    std::string DataLeft;
    Response *clsResponse;
    time_t last_activity; // ADD THIS LINE
    epoll_event event;
} t_clients;

class Server
{
private:
    int _epollInstance;
    int _ServerFd;
    Config &_data;
    struct sockaddr_in _address;
    std::map<int, t_clients> _ClientsMap;
    epoll_event _clients[MAX_CLIENTS_EVENTS];

public:
    int readyClients;
    Server(Config &data);
    int getServerFd();
    void addClientInEppol();
    void readClientRequest(unsigned int clientFd);
    void readCGIPipe(unsigned int pipeFd);
    void sendHttpResponse(int clientFd);
    void CreateEpollInstance();
    void deleteClientFromEpoll(unsigned int clientFd);
    void addNblock(unsigned int clientFd);
    void getReadInfos(unsigned int fd);
    void errorSending(unsigned int fd);
    void ReadSend(unsigned int fd);
    void ifCGI(int fd);
    void modifySockEvents(int epollfd, int fd);
    // void Server::isCGIPipe(int fd);
    void readCGIPipe(int pipeFd);
    void SendErrorPage(int fd, std::string code);
    size_t contentLenght(std::string header);
    void run();
    std::map<int, t_clients> getClients() const;
    ~Server();
};

void throwing(std::string fct);
