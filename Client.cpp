#include "Server.hpp"

void throwing(std::string fct)
{
    throw std::runtime_error(fct + std::string("failed: ") + strerror(errno));
}

void Server::addClientInEppol()
{
    int clientFd;
    clientFd = accept(_ServerFd, NULL, NULL);
    if (clientFd < 0)
        throwing("accept()");
    epoll_event newClient;
    newClient.data.fd = clientFd;
    newClient.events = EPOLLIN & EPOLLOUT;
    if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, clientFd, &newClient) < 0)
        throwing("epoll_ctl()");
    _ClientsFds.push_back(clientFd);
}

void Server::readClientRequest(epoll_event client)
{
    size_t bytesRead;
    char buffer[1024];
    bytesRead = recv(client.data.fd, buffer, sizeof(buffer), 0);
    if (bytesRead == 0)
    {
        epoll_ctl(_epollInstance, EPOLL_CTL_DEL, client.data.fd, NULL);
        close(client.data.fd);
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
    // sendHttpResponse(client.data.fd);
    std::cout << buffer << std::endl;
}