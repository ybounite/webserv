#include "../../includes/Webserv.hpp"
#include "../response/Response.hpp"
#include "../request/RequestHandler.hpp"

void sighandler(int status)
{
    if (status)
        throwing("");
}


Server::Server(Config &data) : _data(data)
{
    int opt = 1;
    signal(SIGINT, sighandler);
    signal(SIGPIPE, SIG_IGN);
    _data = data;
    _ServerFd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(_ServerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (_ServerFd < 0)
        throwing("socket()");

    addNblock(_ServerFd);
    memset((void *)&_address, 0, sizeof(sockaddr_in));
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_data.servers[0].listen_ports[0]);
    if (bind(_ServerFd, (const struct sockaddr *)&_address, sizeof(struct sockaddr_in)) < 0)
        throwing("bind()");
    if (listen(_ServerFd, 1000) < 0)
        throwing("listen()");
}

void Server::CreateEpollInstance()
{
    epoll_event listener;
    listener.events = EPOLLIN;
    listener.data.fd = _ServerFd;
    _epollInstance = epoll_create(1); // we create an epoll instance in the kernel
    if (_epollInstance < 0)
        throwing("epoll_create()");
    if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, _ServerFd, &listener) < 0) // we now watching any event in the server.
        throwing("epoll_ctl()");
}

void Server::run()
{
    CreateEpollInstance();
    std::cout << "Server running on http://localhost:" << _data.servers[0].listen_ports[0] << std::endl;
    while (true)
    {
        readyClients = epoll_wait(_epollInstance, _clients, MAX_CLIENTS_EVENTS, -1);
        if (readyClients < 0)
            throwing("epoll_wait() failed: ");
        for (int i = 0; i < readyClients; i++)
        {
            int fd = _clients[i].data.fd;
            if (fd == _ServerFd)
                addClientInEppol();
            else if (_ClientsMap.find(fd) != _ClientsMap.end() &&
                     _ClientsMap[fd].CGIfd == fd)
                readCGIPipe(fd);
            else
            {
                if (_clients[i].events & EPOLLIN)
                    readClientRequest(fd);
                if (_clients[i].events & EPOLLOUT)
                    sendHttpResponse(fd);
            }
        }
    }
}

// New function to handle CGI pipe reads


std::map<int, t_clients> Server::getClients() const
{
    return _ClientsMap;
}

int Server::getServerFd()
{
    return _ServerFd;
}

Server::~Server()
{
    _ClientsMap.clear();
    close(_ServerFd);
}
