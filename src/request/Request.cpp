/* ************************************************************************** */
/*                    file: Request.cpp                                       */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/16 09:42 by ybounite                   */
/*                    Updated: 2025/11/16 09:42 by ybounite                   */
/* ************************************************************************** */

# include "Request.hpp"


Request::Request( void ) {

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
	std::cout << "* Method : " << _Method << std::endl;
	std::cout << "* URI : " << _URI << std::endl;
	std::cout << "* HTTPversion : " << _HTTPversion << std::endl;
}


void	Request::handleRequest( std::string &raw) {

	std::istringstream	stream(raw);
	std::string		line;

	if (!std::getline(stream, line))
		throw std::runtime_error("invalid request");

	if (!line.empty() && line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);
	parseRequestLine(line);
}
