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
	// Check if client exists in the map
	if (_ClientsMap.find(clientFd) == _ClientsMap.end())
		return;
		
	std::cout << "Client Index [" << clientFd << "]\n";

	ssize_t sendBytes;
	if (_ClientsMap[clientFd].firstTime)
	{
		Request req(_ClientsMap[clientFd].request, _data);
		RequestHandler ReqH(req, _data.servers[0]);
		_ClientsMap[clientFd].clsResponse = new Response(ReqH.HandleMethod());
		_ClientsMap[clientFd].response = _ClientsMap[clientFd].clsResponse->BuildHeaderResponse();
		std::cout << "Header > : " <<  _ClientsMap[clientFd].response << std::endl;
		sendBytes = send(clientFd, _ClientsMap[clientFd].response.c_str(), _ClientsMap[clientFd].response.length(), 0);
		if (sendBytes < 0) {
			deleteClientFromEpoll(clientFd);
			return;
		}
		_ClientsMap[clientFd].firstTime = false;
		return;
	}

	if (_ClientsMap[clientFd].clsResponse == NULL)
	{
		std::cout << "delete bucose response is empty $ " << std::endl;
		deleteClientFromEpoll(clientFd);
		return;
	}

	if (_ClientsMap[clientFd].clsResponse->Fd == -1) {
		if (!_ClientsMap[clientFd].clsResponse->Body.empty())
		{
			sendBytes = send(clientFd, _ClientsMap[clientFd].clsResponse->Body.c_str(), 
							_ClientsMap[clientFd].clsResponse->Body.length(), 0);
			if (sendBytes < 0)
				throwing("send()");
		}
		deleteClientFromEpoll(clientFd);
		return;
	}

	char buffer[1024];
	ssize_t b_read = read(_ClientsMap[clientFd].clsResponse->Fd, buffer, sizeof(buffer));
	if (b_read <= 0)
	{
		if (b_read == 0)
			std::cout << "Full read file \n";
		else
			std::cerr << "Error reading file: " << strerror(errno) << "\n";
		deleteClientFromEpoll(clientFd);
		return;
	}

	sendBytes = send(clientFd, buffer, b_read, 0);
	if (sendBytes < 0)
	{
		std::cout << "delete bucose in send byts is nigative: " << sendBytes << std::endl;
		deleteClientFromEpoll(clientFd);
		return;
	}

	_ClientsMap[clientFd].bytesread += sendBytes;

	if (_ClientsMap[clientFd].bytesread >= static_cast<size_t>(_ClientsMap[clientFd].clsResponse->BodySize))
	{
		std::cout << "Full file sent\n";
		deleteClientFromEpoll(clientFd);
	}
}