#pragma once

#include "../../includes/Webserv.hpp"

class Response;

#define MAX_CLIENTS_EVENTS 1000

class Config;

typedef struct s_clients
{
	s_clients() : fd(-1) ,firstTime(true) ,leftData(false) ,bytesread(0) ,clsResponse(NULL){}
	int			fd;
	std::string	request;
	std::string	response;
	bool		firstTime;
	bool		leftData;
	size_t		bytesread;
	std::string	DataLeft;
	Response   *clsResponse;
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
    void sendHttpResponse(unsigned int clientFd);
    void CreateEpollInstance();
    void deleteClientFromEpoll(unsigned int clientFd);
    void addNblock(unsigned int clientFd);
    void run();
    std::map<int, t_clients> getClients() const;
    ~Server();
};

void throwing(std::string fct);
