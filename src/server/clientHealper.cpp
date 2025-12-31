#include "../../includes/Webserv.hpp"


void Server::modifySockEvents(int epollfd, int fd)
{
	epoll_event newClient;
	newClient.data.fd = fd;
	newClient.events = EPOLLIN | EPOLLOUT;
	// EPOLL_CTL_MOD this flag tell the kernel to modify the the event of the FD
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &newClient);
}



void Server::deleteClientFromEpoll(unsigned int clientFd)
{
	epoll_ctl(_epollInstance, EPOLL_CTL_DEL, clientFd, NULL);
	close(clientFd);
	if (_ClientsMap[clientFd].clsResponse != NULL)
	{
		if (_ClientsMap[clientFd].clsResponse->Fd != -1)
			close(_ClientsMap[clientFd].clsResponse->Fd);
		delete _ClientsMap[clientFd].clsResponse;
	}
	_ClientsMap.erase(clientFd);
}



size_t Server::contentLenght(std::string header)
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




void Server::getReadInfos(unsigned int fd)
{
	size_t sendBytes;
	Request req(_ClientsMap[fd].request, _data);
	RequestHandler ReqH(req, _data.servers[0]);
	_ClientsMap[fd].clsResponse = new Response(ReqH.HandleMethod());
	_ClientsMap[fd].response = _ClientsMap[fd].clsResponse->BuildHeaderResponse();
	sendBytes = send(fd, _ClientsMap[fd].response.c_str(), _ClientsMap[fd].response.length(), 0);
	if (sendBytes < 0)
	{
		deleteClientFromEpoll(fd);
		throwing("send()");
	}
	_ClientsMap[fd].firstTime = false;
}





void Server::errorSending(unsigned int fd)
{
	size_t sendBytes;
	if (!_ClientsMap[fd].clsResponse->Body.empty())
	{
		sendBytes = send(fd, _ClientsMap[fd].clsResponse->Body.c_str(),
						 _ClientsMap[fd].clsResponse->Body.length(), 0);
		if (sendBytes < 0)
			throwing("send()");
	}
	deleteClientFromEpoll(fd);
}




void Server::ReadSend(unsigned int fd)
{
	char buffer[1024];
	size_t sendBytes;
	ssize_t b_read = read(_ClientsMap[fd].clsResponse->Fd, buffer, sizeof(buffer));
	if (b_read <= 0)
	{
		if (b_read == 0)
			std::cout << "Full read file \n";
		else
			std::cerr << "Error reading file: " << strerror(errno) << "\n";
		deleteClientFromEpoll(fd);
		throwing("read()");
	}

	sendBytes = send(fd, buffer, b_read, 0);
	if (sendBytes < 0)
	{
		std::cout << "delete bucose in send byts is nigative: " << sendBytes << std::endl;
		deleteClientFromEpoll(fd);
		throwing("send()");
	}
	_ClientsMap[fd].bytesread += sendBytes;
}




