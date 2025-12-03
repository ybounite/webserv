/* ************************************************************************** */
/*                    file: Request.cpp                                       */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/16 09:42 by ybounite                   */
/*                    Updated: 2025/11/16 09:42 by ybounite                   */
/* ************************************************************************** */

# include "Request.hpp"
# include "RequestHandler.hpp"


Request::Request( void ) {

}

Request::Request( std::string raw ) {
	this->handleRequest(raw);
}


Request::Request( const  Request &Other ) {
	*this = Other;
}

Request &Request::operator=( const  Request &Other ) {
	if (this != &Other) {
		_Method = Other._Method; 
		_URI = Other._URI;
		_HTTPversion = Other._HTTPversion;
		_Headers = Other._Headers;
		_Body = Other._Body;
		_Path = Other._Path;	
	}
	return *this;
}

Request::~Request( void ) {

}

std::string	Request::getMethod( void ) {return _Method;}
std::string	Request::getUri( void ) { return _URI;}
std::string	Request::getHTTPversion( void ) {return _HTTPversion;}
std::map<std::string , std::string>	&Request::getHeaders( void ) { return _Headers;}
std::string	Request::getBody( void ) { return _Body;}
std::string	Request::getPath( void ) { return _Path;}

void	Request::parseRequestLine( const std::string &line ) {
	std::istringstream ss(line);
	ss >> _Method >> _URI >> _HTTPversion;
	// std::cout << GREEN << "**********************" << RESET << std::endl;
	// std::cout << "* Method : " << _Method << std::endl;
	// std::cout << "* URI : " << _URI << std::endl;
	// std::cout << "* HTTPversion : " << _HTTPversion << std::endl;
	// std::cout << GREEN << "**********************" << RESET << std::endl;
}

void	Request::parseHeaders( std::istringstream &stream ) {
	std::cout << GREEN << "Hendling HTTP request" << RESET << std::endl;
	std::string	Line;
	while (getline(stream, Line)) {

		if (!Line.empty() && Line[Line.length() - 1] == '\r') // this '\r' trailing 
			Line.erase(Line.length() - 1);

		if (Line.empty())
			break ;
		size_t	pos = Line.find(":");
		if (pos == std::string::npos) throw std::runtime_error("invalid header field");

		std::string	key = Line.substr(0, pos);
		std::string	value = Line.substr(pos + 1);

		if (!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		// std::cout << "this key :" << key << std::endl;
		// std::cout << "this value :" << value << std::endl;
		this->_Headers[key] = value;
	}
}

void	printRequest(std::string &row) {
	std::cout << YELLOW << "****************Request************" << RESET << std::endl;
	std::cout << row << std::endl;
	std::cout << YELLOW << "****************END****************" << RESET << std::endl;
}

/*
		* ⚠ Important limitations
	✔ This function only works for Content-Length, not for:
	✔ Transfer-Encoding: chunked
	✔ multipart form decoding
	✔ file uploads (multipart/form-data)
	✔ I can show you how to add support for those too.
*/

void	Request::parseBody( std::istringstream &stream ){

	std::cout << GREEN << "Bady HTTP request" << RESET << std::endl;
	std::string contentLengthStr = _Headers["Content-Length"];
	if (contentLengthStr.empty()) 
		return;

	size_t contentLength = std::atoi(contentLengthStr.c_str());
	_Body.resize(contentLength);

	stream.read(&_Body[0], contentLength);
	std::cout << _Body << std::endl;
}

void	Request::handleRequest( std::string &raw) {
	std::istringstream	stream(raw);
	std::string		line;
	printRequest(raw);

	if (!std::getline(stream, line))
		throw std::runtime_error("invalid request");

	if (!line.empty() && line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);

	parseRequestLine(line);
	parseHeaders(stream); 
	parseBody(stream);
}

std::string		Request::response(){

	RequestHandler	rqshd(*this);
	std::string Body = rqshd.handle(GbConfig);
	return Body;
}

// void	Request::sendResponse(int clientFd) {

// 	RequestHandler	rqshd(*this);
// 	std::string Body = rqshd.handle(GbConfig);
// 	send(clientFd, Body.c_str(), Body.length(), 0);
// }
