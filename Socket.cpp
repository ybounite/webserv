#include "Socket.hpp"

bool running = 0;

void handler(int)
{
    running = 1;
}
void throwing(std::string fct)
{
    throw std::runtime_error(fct + std::string("failed: ") + strerror(errno));
}

Socket::Socket()
{
    // signal(SIGINT, handler);
    _SockeFd = socket(AF_INET, SOCK_STREAM, 0);
    memset((void *)&_address, 0, sizeof(sockaddr_in));
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(8401);

    if (_SockeFd < 0)
        throwing("socket()");
    if (bind(_SockeFd, (const struct sockaddr *)&_address, sizeof(struct sockaddr_in)) < 0)
        throwing("bind()");
    if (listen(_SockeFd, 1) < 0)
        throwing("listen()");
}

void Socket::sendHttpResponse(int clientFd)
{
    std::cout << "ilyass" << std::endl;
    const char *body =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<!DOCTYPE html>\r\n"
        "<html>\r\n"
        "<head>\r\n"
        "<title>Hello</title>\r\n"
        "</head>\r\n"
        "<body>\r\n"
        "<h1>Hello</h1>\r\n"
        "</body>\r\n"
        "</html>\r\n";
    // Send header then body
    send(clientFd, body, strlen(body), 0);
}

void Socket::addClientInEppol()
{
    int clientFd;
    clientFd = accept(_SockeFd, NULL, NULL);
    if (clientFd < 0)
        throwing("accept()");
    epoll_event newClient;
    newClient.data.fd = clientFd;
    newClient.events = EPOLLIN;
    if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, clientFd, &newClient) < 0)
        throwing("epoll_ctl()");
    t_clients c;
    c.id = clientFd;
    _ClientsMap[clientFd] = c;
}

void Socket::readClientRequest(epoll_event client)
{
    size_t bytesRead;
    char buffer[1024];

    bytesRead = recv(client.data.fd, buffer, sizeof(buffer), 0);
    if (bytesRead == 0)
    {
        epoll_ctl(_epollInstance, EPOLL_CTL_DEL, client.data.fd, NULL);
        close(client.data.fd);
        _ClientsMap.erase(client.data.fd);
        return;
    }
    else if (bytesRead < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        else
            throwing("recv()");
    }
    else
        buffer[bytesRead] = '\0';
    std::cout << buffer << std::endl;
    // parsaingObject(std::string(buffer));
    sendHttpResponse(client.data.fd);

    close(client.data.fd);
    _ClientsMap.erase(client.data.fd);
}

void Socket::run()
{
    int readyClients;
    epoll_event listener;
    listener.events = EPOLLIN;
    listener.data.fd = _SockeFd;
    epoll_event clients[MAX_CLIENTS_EVENTS];
    _epollInstance = epoll_create1(EPOLL_CLOEXEC);
    if (_epollInstance < 0)
        throwing("epoll_create()");
    if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, _SockeFd, &listener) < 0)
        throwing("epoll_ctl()");
    while (true)
    {
        readyClients = epoll_wait(_epollInstance, clients, MAX_CLIENTS_EVENTS, -1);
        if (readyClients < 0)
            throwing("epoll_wait() failed: ");
        for (int i = 0; i < readyClients; i++)
        {
            if (clients[i].data.fd == _SockeFd)
                addClientInEppol();
            else if (clients[i].events & EPOLLIN)
                readClientRequest(clients[i]);
        }
    }
}

std::map<int, t_clients> Socket::getClients() const
{
    return _ClientsMap;
}

int Socket::getSocketFd()
{
    return _SockeFd;
}

Socket::~Socket()
{
    close(_SockeFd);
}