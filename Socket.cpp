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
        sockaddr_in clientSockAdd;
        socklen_t len = sizeof(sockaddr_in);
        _ClientFd = accept(_SockeFd, (struct sockaddr *)&clientSockAdd, &len);
        if (_ClientFd < 0)
            throw std::runtime_error(std::string("accept() failed:") + strerror(errno));
        send(_ClientFd, "hello from server hiihihihi.\n", 30, 0);
        recv(_ClientFd, _buffer, sizeof(_buffer), 0);
        std::cout << "server receive: " << _buffer << std::endl;
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