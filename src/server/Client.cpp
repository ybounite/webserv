#include "../../includes/Webserv.hpp"
#include "../response/Response.hpp"
#include "../request/RequestHandler.hpp"

void Server::addClientInEppol()
{
	int clientFd;
	// accept the client (that create for him a new socket and return a FD for it)
	clientFd = accept(_ServerFd, NULL, NULL);
	// make this socket a nonblocked one so the recv wont block
	addNblock(clientFd);
	if (clientFd < 0)
		throwing("accept()");
	epoll_event newClient;
	newClient.data.fd = clientFd;
	newClient.events = EPOLLIN;
	if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, clientFd, &newClient) < 0)
		throwing("epoll_ctl()");
	t_clients client;
	client.fd = clientFd;
	_ClientsMap[clientFd] = client;
}

// this function add the EPOLLOUT flag to the socket in the epoll instance to watch also if the client is ready to recv data
void Server::readClientRequest(unsigned int clientFd)
{
	int bytesRead;
	char buffer[1024];
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
	_ClientsMap[clientFd].last_activity = time(NULL);
	modifySockEvents(_epollInstance, clientFd);
}

void Server::sendHttpResponse(unsigned int clientFd)
{
	time_t current_time = time(NULL);
	// if (_ClientsMap[clientFd].CGIfd > -1)
	// {
	// 	Msg::error("heree\n");
	// 	char buffer[1024];
	// 	ssize_t sendBytes;
	// 	ssize_t b_read = read(_ClientsMap[clientFd].CGIfd, buffer, sizeof(buffer));
	// 	if (b_read < 0)
	// 	{
	// 		deleteClientFromEpoll(clientFd);
	// 		deleteClientFromEpoll(_ClientsMap[clientFd].fd);
	// 		throwing("read()");
	// 	}
	// 	if (b_read == 0)
	// 	{
	// 		std::string std = to_string(b_read) + "\r\n\r\n";
	// 		sendBytes = send(clientFd, std.c_str(), std.length(), 0);
	// 		deleteClientFromEpoll(clientFd);
	// 		deleteClientFromEpoll(_ClientsMap[clientFd].fd);
	// 		return;
	// 	}
	// 	std::string std = to_string(b_read) + "\r\n" + buffer + "\r\n";
	// 	sendBytes = send(clientFd, std.c_str(), std.length(), 0);
	// 	if (sendBytes < 0)
	// 	{
	// 		std::cout << "delete bucose in send byts is nigative: " << sendBytes << std::endl;
	// 		deleteClientFromEpoll(clientFd);
	// 		deleteClientFromEpoll(_ClientsMap[clientFd].fd);
	// 		throwing("send()");
	// 	}
	// 	return;
	// }
	if (current_time - _ClientsMap[clientFd].last_activity > 10) // 30 seconds timeout
	{
		std::cout << "Client timeout, closing connection\n";
		deleteClientFromEpoll(clientFd);
		return;
	}
	if (_ClientsMap[clientFd].firstTime)
		return getReadInfos(clientFd);

	if (_ClientsMap[clientFd].clsResponse->Fd == -1)
		return errorSending(clientFd);

	ReadSend(clientFd);

	if (_ClientsMap[clientFd].bytesread >= _ClientsMap[clientFd].clsResponse->BodySize)
	{
		deleteClientFromEpoll(clientFd);
		_ClientsMap[clientFd].last_activity = time(NULL);
	}
}