#include "../../includes/Webserv.hpp"
#include "../response/Response.hpp"
#include "../request/RequestHandler.hpp"

bool running = 0;

void sighandler(int status)
{
    if (status)
        throwing("");
}

void Server::addNblock(unsigned int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        throwing("fcntl(F_GETFL)");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        throwing("fcntl(F_SETFL)");
}

Server::Server(Config &data) : _data(data)
{
    int opt = 1;
    signal(SIGINT, sighandler);
    _data = data;
    _ServerFd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(_ServerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    addNblock(_ServerFd);
    memset((void *)&_address, 0, sizeof(sockaddr_in));
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_data.servers[0].listen_port);

    if (_ServerFd < 0)
        throwing("socket()");
    if (bind(_ServerFd, (const struct sockaddr *)&_address, sizeof(struct sockaddr_in)) < 0) // now we assign to this socket a port and an address family to get conection with.
        throwing("bind()");
    if (listen(_ServerFd, 1000) < 0) // now the server is listening for new requests.
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
    std::cout << "Server running on http://localhost:" << _data.servers[0].listen_port << std::endl;
    while (true)
    {
        readyClients = epoll_wait(_epollInstance, _clients, MAX_CLIENTS_EVENTS, -1);
        if (readyClients < 0)
            throwing("epoll_wait() failed: ");
        for (int i = 0; i < readyClients; i++)
        {
            int fd = _clients[i].data.fd;
            if (_clients[i].data.fd == _ServerFd) // a new client request need to be accepted.
                addClientInEppol();
            // else
            // {
                
                // Check if this is a CGI pipe FD
                // if (_ClientsMap.find(fd) != _ClientsMap.end() && _ClientsMap[fd].CGIfd == fd)
                // {
                //     // This is a CGI pipe FD
                //     if (_clients[i].events & EPOLLIN)
                //         readCGIPipe(fd);
                // }
                else
                {
                    // This is a regular client socket FD
                    if (_clients[i].events & EPOLLIN) // do client ready to  send data to the server ?
                        readClientRequest(fd);
                    if (_clients[i].events & EPOLLOUT) // if the client send a request this condition would be true and i will respond here
                        sendHttpResponse(fd);
                }
            // }
        }
    }
}

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
