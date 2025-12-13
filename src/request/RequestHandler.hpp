/* ************************************************************************** */
/*                    file: RequestHandler.hpp                                */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:09 by ybounite                   */
/*                    Updated: 2025/11/29 16:09 by ybounite                   */
/* ************************************************************************** */

# pragma once
# include "../../includes/Webserv.hpp"
# include "../response/Response.hpp"

/* ************************************************************************** */
/*                          RequestHandler Class                              */
/* ************************************************************************** */

// class	ConfigFileReader;
// class	Request;

class RequestHandler : public Request {

public:

	RequestHandler( void );
	RequestHandler( const Request &Other );
	// RequestHandler( const  RequestHandler &Other );
	~RequestHandler( void );

	std::string		handle(const Config &config);
	std::string		buildHttpResponse(int statusCode, const std::string &body);
private:
	// Response		rsp;
	// Response		handleGET(const Request req, const ServerConfig &config);
};
