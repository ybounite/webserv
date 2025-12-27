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
    fcntl(clientFd, F_SETFL, O_NONBLOCK);
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

    index += 16;
    std::string str = header.substr(index);

    size_t index2 = str.find("\n");
    if (index2 == std::string::npos)
        return 0;

    std::string Clenght = str.substr(index, index2);
    return (atoi(Clenght.c_str()));
}

void Server::readClientRequest(unsigned int clientFd)
{
    int bytesRead;
    char buffer[_data.servers[0].client_max_body_size];
    // while (true)
    // {
        bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead == 0)
        {
            deleteClientFromEpoll(clientFd);
            return;
        }
        else if (bytesRead < 0)
        {
            //  if recv returns -1 and the errno code is on of those . that's mean there is no data yet and the recv would block.
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            else
                throwing("recv()");
        }
        _ClientsMap[clientFd].request.append(buffer, bytesRead);
        size_t headerEnd = _ClientsMap[clientFd].request.find("\r\n\r\n");
        if (headerEnd == std::string::npos)
            return;
        if (contentLenght(_ClientsMap[clientFd].request) > _ClientsMap[clientFd].request.length() - headerEnd + 4)
            return;
        printf("===========================================================\n");
        std::cout << _ClientsMap[clientFd].request << std::endl;
        printf("===========================================================\n");
    //     try
    //     {
    //         Request req(_ClientsMap[clientFd].request, _data);
    //         break; // request complete
    //     }
    //     catch (const std::exception &e)
    //     {
    //         printf("%s\n", e.what());
    //     }
    // }
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