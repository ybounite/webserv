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

# define LINE_BREAK "\r\n"
class Response;

class Request {
public:

	enum	enStatusRequest{ enVALID, enINVALID, enMISSING_LINE, enMALFORMED_LINE, enMISSING_HEADER, enMISSING_BODY};

	Request( void );
	Request( std::string &raw, Config &ConfigFile );
	Request( const  Request &Other );
	Request &operator=( const  Request &Other );
	~Request();
	
	enStatusRequest						status;
	std::map<std::string, std::string> cookies;

	const std::string							&getMethod( void ) const;
	const std::string							&getUri( void ) const;
	const std::string							&getHTTPversion( void ) const;
	const std::map<std::string, std::string>	&getHeaders( void ) const;
	const std::string							&getBody( void ) const;
	const std::string							&getPath( void ) const;
	void								setHeader(const std::string &key, const std::string &value);
	void								handleRequest( std::string &raw );
	void								CreateSessioncookies();
	void 								createNewSession(ServerConfig &config);

	void								parseRequestLine( const std::string &line );
	void								ParseHeaders( std::istringstream &stream );
	void								ParseBody( std::istringstream &stream );
	void								ParseChunked( std::istringstream &stream ); //
	std::string							response( void );
	std::string							getHeader(const std::string &key) const;
	size_t								getContentLength() const;
	///
	void ParseCookies();

private:
	std::string							_Method;
	std::string							_URI;
	std::string							_Protocol;
	std::map<std::string, std::string>	_Headers;
	std::string							_Body;
	std::string							_Path;
	Config								_config;
	bool								is_ValidRequest() const;
};
