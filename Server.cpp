#include "Server.hpp"

bool running = 0;

Server::Server()
{
    _ServerFd = socket(AF_INET, SOCK_STREAM, 0);
    memset((void *)&_address, 0, sizeof(sockaddr_in));
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(34023);

    if (_ServerFd < 0)
        throwing("socket()");
    if (bind(_ServerFd, (const struct sockaddr *)&_address, sizeof(struct sockaddr_in)) < 0)
        throwing("bind()");
    if (listen(_ServerFd, 1) < 0)
        throwing("listen()");
}

void Server::CreateEpollInstance()
{
    epoll_event listener;
    listener.events = EPOLLIN;
    listener.data.fd = _ServerFd;
    _epollInstance = epoll_create1(EPOLL_CLOEXEC);
    if (_epollInstance < 0)
        throwing("epoll_create()");
    if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, _ServerFd, &listener) < 0)
        throwing("epoll_ctl()");
}

void Server::run()
{
    CreateEpollInstance();
    while (true)
    {
        readyClients = epoll_wait(_epollInstance, _clients, MAX_CLIENTS_EVENTS, -1);
        if (readyClients < 0)
            throwing("epoll_wait() failed: ");
        for (int i = 0; i < readyClients; i++)
        {
            if (_clients[i].data.fd == _ServerFd) // a new client request need to be accepted.
                addClientInEppol();
            else if (_clients[i].events & EPOLLIN)
            {
                std::cout << "here" << std::endl;
                readClientRequest(_clients[i]);

            } // client wanna send data to the server.
            // else if (_clients[i].events & EPOLLOUT)// client ready to recieve response server
            //     sendHttpResponse(_clients[i].data.fd);
            // send the response in this case //
        }
    }
}

std::vector<int> Server::getClients() const
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

void Server::sendHttpResponse(int clientFd)
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