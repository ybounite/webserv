#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

class Socket
{
private:
    int _SockeFd;
    int _ClientFd;
    char _buffer[1024];
    struct sockaddr_in address;

public:
    Socket();
    int getSocketFd();
    ~Socket();
};

#endif