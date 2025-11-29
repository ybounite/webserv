#include "../../includes/Webserv.hpp"

bool running = 0;

Server::Server(Config &data): _data(data)
{
    _data = data;
    _ServerFd = socket(AF_INET, SOCK_STREAM, 0);
    memset((void *)&_address, 0, sizeof(sockaddr_in));
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_data.servers[0].listen_port);

    if (_ServerFd < 0)
        throwing("socket()");
    if (bind(_ServerFd, (const struct sockaddr *)&_address, sizeof(struct sockaddr_in)) < 0) // now we assign to this socket a port and an address family to get conection with.
        throwing("bind()");
    if (listen(_ServerFd, 1) < 0) // now the server is listening for new requests.
        throwing("listen()");
}

void Server::CreateEpollInstance()
{
    epoll_event listener;
    listener.events = EPOLLIN | EPOLLOUT;
    listener.data.fd = _ServerFd;
    _epollInstance = epoll_create1(EPOLL_CLOEXEC); // we create an epoll instance in the kernel
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
            if (_clients[i].data.fd == _ServerFd) // a new client request need to be accepted.
                addClientInEppol();
            else if (_clients[i].events & EPOLLIN) // client wanna send data to the server.
                readClientRequest(_clients[i]);   // here i read what the client sent
        }
    }
}

std::map<int, int> Server::getClients() const
{
    return _ClientsFds;
}

int Server::getServerFd()
{
    return _ServerFd;
}

Server::~Server()
{
    _ClientsFds.clear();
    close(_ServerFd);
}
