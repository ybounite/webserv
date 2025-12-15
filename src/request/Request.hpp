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
	Request( std::string raw, Config &ConfigFile );
	Request( const  Request &Other );
	Request &operator=( const  Request &Other );
	~Request();
	
	const std::string							&getMethod( void ) const;
	const std::string							&getUri( void ) const;
	const std::string							&getHTTPversion( void ) const;
	const std::map<std::string, std::string>	&getHeaders( void ) const;
	const std::string							&getBody( void ) const;
	const std::string							&getPath( void ) const;

	void								handleRequest( std::string &raw );
	void								parseRequestLine( const std::string &line );
	void								parseHeaders( std::istringstream &stream );
	void								parseBody( std::istringstream &stream );
	std::string							response( void );
	//void								sendResponse(int clientFd);
	std::string							getHeader(const std::string &key) const;
	size_t								getContentLength() const;

private:
	std::string							_Method; // GET , POST DELETE
	std::string							_URI; // /index.html
	std::string							_HTTPversion;
	std::map<std::string, std::string>	_Headers;
	std::string							_Body;
	std::string							_Path;
	Config								_config; // Store config as a member, not inherit from it
	//	std::string				_Server;
};
