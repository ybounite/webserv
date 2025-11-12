#include "Socket.hpp"

Socket::Socket()
{
    _SockeFd = socket(AF_INET, SOCK_STREAM, 0);
    memset((void *)&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (_SockeFd < 0)
        throw std::runtime_error(std::string("socket() failed: ") + strerror(errno));
    if (bind(_SockeFd, (const struct sockaddr *)&address, sizeof(struct sockaddr_in)))
        throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
    if (listen(_SockeFd, 1) < 0)
        throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
    while (true)
    {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int clientFd = accept(_SockeFd, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientFd < 0)
        {
            std::cerr << "accept() failed: " << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "Client connected!" << std::endl;

        // Simple response
        const char *msg = "Hello from server!\n";
        send(clientFd, msg, strlen(msg), 0);

        close(clientFd);
    }
}

int Socket::getSocketFd()
{
    return _SockeFd;
}

Socket::~Socket()
{
    close(_SockeFd);
}