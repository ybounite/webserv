/* ************************************************************************** */
/*                    file: RequestHandler.hpp                                */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:09 by ybounite                   */
/*                    Updated: 2025/11/29 16:09 by ybounite                   */
/* ************************************************************************** */

# pragma once
# include "../../includes/Webserv.hpp"

class Response;

/* ************************************************************************** */
/*                          RequestHandler Class                              */
/* ************************************************************************** */

class RequestHandler {
public:
	enum	enHttpMethod { HTTP_GET, HTTP_POST, HTTP_DELETE, HTTP_UNKNOWN };
	// Static handler - processes Request and returns Response
	static Response		handle( const Request &req, const Config &config );
	static short		getMothod( const std::string &method );

private:
	// Helper methods for different HTTP methods
	static Response		handleGET(const Request &req, const ServerConfig &config);
	static Response		handlePOST(const Request &req, const ServerConfig &config);
	static Response		handleDELETE(const Request &req, const ServerConfig &config);
	
	// Utility functions
	static std::string	readFile(const std::string &path);
	static std::string	getErrorPage(int statusCode);
};
