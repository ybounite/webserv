#include "../../includes/Webserv.hpp"
#include "../response/Response.hpp"
#include "../request/RequestHandler.hpp"

void Server::SendErrorPage(int sockfd, std::string code)
{
    std::cout << "------------------------" << std::endl;
    std::ostringstream errorPath;
    std::ostringstream oss;
    errorPath << "errors/" << code << ".html";
    int fd = open(errorPath.str().c_str(), O_RDONLY);
    char buffer[2048];
    int bytes = read(fd, buffer, sizeof(buffer));
    buffer[bytes] = '\0';
    std::string body = buffer;
    oss << body.size();
    std::string header =
        "HTTP/1.1 " + code + " OK\r\nContent-Type: text/html\r\nContent-Length: " + oss.str() + "\r\nConnection: close\r\n\r\n";
    header += body;
    send(sockfd, header.c_str(), header.length(), 0);
}

void Server::addNblock(unsigned int fd)
{
    if ((int)fd < 0)
        return;

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        throwing("fcntl(F_GETFL)");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        throwing("fcntl(F_SETFL)");
}

void Server::readCGIPipe(int pipeFd)
{
    int sock = _ClientsMap[pipeFd].fd;
    char buffer[1024];
    ssize_t b_read = read(pipeFd, buffer, sizeof(buffer));

    std::cout << "Read from CGI pipe " << pipeFd << " : " << b_read << " bytes" << std::endl;

    if (b_read < 0)
    {
        std::cout << "bread";
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        SendErrorPage(sock, "500");
        return;
    }

    if (b_read == 0)
    {
        std::cout << "EOF from CGI pipe - closing connection\n";
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        return;
    }

    // Refresh activity timestamp since we successfully read data from CGI
    _ClientsMap[pipeFd].last_activity = time(NULL);

    // Forward CGI output directly to client (don't wrap in chunks)
    ssize_t sendBytes = send(sock, buffer, b_read, MSG_NOSIGNAL);

    if (sendBytes < 0)
    {
        std::cout << "sendbytes";
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        SendErrorPage(sock, "500");
        return;
    }
    if (time(NULL) - _ClientsMap[pipeFd].last_activity > 10) // 30 seconds timeout
    {
        std::cout << "Client timeout, closing connection\n";
        deleteClientFromEpoll(pipeFd);
        deleteClientFromEpoll(sock);
        SendErrorPage(sock, "408");
        return;
    }
    std::cout << "Sent " << sendBytes << " bytes to client" << std::endl;
}