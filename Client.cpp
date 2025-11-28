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
    int flags = fcntl(clientFd, F_GETFL, 0);
    fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
    epoll_event newClient;
    newClient.data.fd = clientFd;
    newClient.events = EPOLLIN;
    if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, clientFd, &newClient) < 0)
        throwing("epoll_ctl()");
    _ClientsFds[clientFd] = clientFd;
}

void Server::readClientRequest(epoll_event client)
{
    int bytesRead;
    char buffer[1024];
    bytesRead = recv(client.data.fd, buffer, sizeof(buffer), 0);
    if (bytesRead == 0)
    {
        epoll_ctl(_epollInstance, EPOLL_CTL_DEL, client.data.fd, NULL);
        close(client.data.fd);
        _ClientsFds.erase(client.data.fd);
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
}

void Server::sendHttpResponse(int clientFd)
{
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
    send(clientFd, body, strlen(body), 0);
    close(clientFd);
    _ClientsFds.erase(clientFd);
}