/* ************************************************************************** */
/*                    file: Request.hpp                                       */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/16 09:36 by ybounite                   */
/*                    Updated: 2025/11/16 09:36 by ybounite                   */
/* ************************************************************************** */

# pragma once
# include "../../includes/Webserv.hpp"

/* ************************************************************************** */
/*                          Request Class                                     */
/* ************************************************************************** */
// class	RequestHandler;

# define LINE_BREAK "\r\n"

class Request {
public:

	Request( void );
	Request( std::string raw );
	Request( const  Request &Other );
	Request &operator=( const  Request &Other );
	~Request( void );
	
	std::string	getMethod( void );
	std::string	getUri( void );
	std::string	getHTTPversion( void );
	std::map<std::string , std::string>	&getHeaders( void );
	std::string	getBody( void );
	std::string	getPath( void );

	void	handleRequest( std::string &raw );
	void	parseRequestLine( const std::string &line );
	void	parseHeaders( std::istringstream &stream );
	void	parseBody(std::istringstream &stream);
	void	sendResponse(int clientFd);
private:
	std::string							_Method; // GET , POST DELETE
	std::string							_URI; // /index.html
	std::string							_HTTPversion;
//	std::string				_Server;
	std::map<std::string, std::string>	_Headers;
	std::string							_Body;
	std::string							_Path;
};
