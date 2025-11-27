# pragma once
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

