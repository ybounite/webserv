#include "../../includes/Webserv.hpp"

void throwing(std::string fct)
{
    throw std::runtime_error(fct + std::string("failed: ") + strerror(errno));
}

void Server::addClientInEppol()
{
    int clientFd;
    // accept the client (that create for him a new socket and return a FD for it)
    clientFd = accept(_ServerFd, NULL, NULL);
    // make this socket a nonblocked one so the recv wont block
    addNblock(clientFd);
    if (clientFd < 0)
        throwing("accept()");
    // create new client epoll event
    epoll_event newClient;
    newClient.data.fd = clientFd;
    newClient.events = EPOLLIN;
    if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, clientFd, &newClient) < 0)
        throwing("epoll_ctl()");
    // store the client FD and REQ/RES buffer in this struct
    t_clients client;
    client.fd = clientFd;
    // add it to the map so we can deal with it smoothly
    _ClientsMap[clientFd] = client;
}

// this function add the EPOLLOUT flag to the socket in the epoll instance to watch also if the client is ready to recv data
void modifySockEvents(int epollfd, int fd)
{
    epoll_event newClient;
    newClient.data.fd = fd;
    newClient.events = EPOLLIN | EPOLLOUT;
    // EPOLL_CTL_MOD this flag tell the kernel to modify the the event of the FD
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &newClient);
}

void Server::deleteClientFromEpoll(unsigned int clientFd)
{
    // delete the client FD from the epoll instance so we are no longer watching it
    epoll_ctl(_epollInstance, EPOLL_CTL_DEL, clientFd, NULL);
    close(clientFd);
    _ClientsMap.erase(clientFd);
}

size_t contentLenght(std::string header)
{
    size_t index = header.find("Content-Length:");
    if (index == std::string::npos)
        return 0;

    index += 15; // "Content-Length:"

    // Skip any spaces after the colon
    while (index < header.length() && header[index] == ' ')
        index++;

    size_t endIndex = header.find("\r\n", index);
    if (endIndex == std::string::npos)
        return 0;

    std::string Clenght = header.substr(index, endIndex - index);
    return atoi(Clenght.c_str());
}

void Server::readClientRequest(unsigned int clientFd)
{
    int bytesRead;
    char buffer[100];
    bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead == 0)
        return (deleteClientFromEpoll(clientFd));
    else if (bytesRead < 0)
        return;
    _ClientsMap[clientFd].request.append(buffer, bytesRead);
    size_t headerEnd = _ClientsMap[clientFd].request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return;
    if (contentLenght(_ClientsMap[clientFd].request) > _ClientsMap[clientFd].request.length() - headerEnd - 4)
        return;
    modifySockEvents(_epollInstance, clientFd);
}

void Server::sendHttpResponse(unsigned int clientFd)
{
    try
    {
        Request req(_ClientsMap[clientFd].request, _data);
        Response resp = RequestHandler::handle(req, _data);
        resp.send(clientFd);
        deleteClientFromEpoll(clientFd);
    }

    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}