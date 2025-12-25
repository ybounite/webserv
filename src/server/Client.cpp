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

void Server::readClientRequest(unsigned int clientFd)
{
    int bytesRead;
    char buffer[_data.servers[0].client_max_body_size];
    std::string str;
    // size_t index;
    std::string boundary;
    std::string next;
    int i = 0;
    // recv what the client sent to the server.
    while (true)
    {
        bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
        buffer[bytesRead] = '\0';
        str += buffer;
        try
        {
            std::cout << str << std::endl;
            Request req(str, _data);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
        std::cout << "heheheehhiya ======> " <<  i << std::endl;
        if (i++ == 10)
            break;
    }
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
    else
        buffer[bytesRead] = '\0';
    _ClientsMap[clientFd].request += buffer;
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