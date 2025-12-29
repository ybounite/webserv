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
	static const ServerConfig &server;
	// Static handler - processes Request and returns Response
	static Response		handle( const Request &req, const Config &config );
	static short		getMothod( const std::string &method );
	static Response		BuildErrorResponse( short code );

private:
	// Helper methods for different HTTP methods
	static Response		handleGET(const Request &req, const ServerConfig &config);
	static Response		handlePOST(const Request &req, const ServerConfig &config);
	static Response		handleDELETE(const Request &req, const ServerConfig &config);
	
	// Utility functions
	static std::string			readFile(const std::string &path);
	static std::string			getErrorPage(int statusCode);
	//static std::string			_BuildFileSystemPath(std::string Root, std::string Uri);
	static std::string			_BuildFileSystemPath(const std::string &root, const std::string &uri);

	static bool					_ResourceExists( std::string &Path );
	//static std::string			_ResolveIndexFile( std::string Path, const ServerConfig &server );
	static std::string			_ResolveIndexFile(const std::string &path, const ServerConfig &server, const LocationConfig &loc);
	static Response 			serveFile(const std::string &path);
	static Response 		generateAutoindexResponse(const std::string &dirPath, const std::string &uri);
};
