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
    if ((int)fd < 0)
        throwing("addNblock(): invalid fd");

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
    /* Prevent the process from being killed by SIGPIPE when writing to closed sockets */
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
    _address.sin_port = htons(_data.servers[0].listen_port);
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
void Server::readCGIPipe(int pipeFd)
{
    int sock = _ClientsMap[pipeFd].fd;
    char buffer[1024];
    ssize_t b_read = read(pipeFd, buffer, sizeof(buffer));

    std::cout << "Read from CGI pipe " << pipeFd << " : " << b_read << " bytes" << std::endl;

    if (b_read < 0)
    {
        std::cout << "Error reading pipe: " << std::endl;
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        return;
    }

    if (b_read == 0)
    {
        std::cout << "EOF from CGI pipe - child process finished" << std::endl;
        const char *final_chunk = "\r\n\r\n";
        send(sock, final_chunk, strlen(final_chunk), MSG_NOSIGNAL);
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        return;
    }

    // Refresh activity timestamp since we successfully read data from CGI
    _ClientsMap[pipeFd].last_activity = time(NULL);
    ssize_t sendBytes = send(sock, buffer, b_read, MSG_NOSIGNAL);
    if (sendBytes < 0)
    {
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        return;
    }
    if (time(NULL) - _ClientsMap[pipeFd].last_activity > 10) // 30 seconds timeout
    {
        std::cout << "Client timeout, closing connection\n";
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        return;
    }
    std::cout << "Sent " << sendBytes << " bytes to client" << std::endl;
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
