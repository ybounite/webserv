/* ************************************************************************** */
/*                    file: Response.hpp                                      */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:02 by ybounite                   */
/*                    Updated: 2025/11/29 16:02 by ybounite                   */
/* ************************************************************************** */

# pragma once
# include "../../includes/Webserv.hpp"

/* ************************************************************************** */
/*                          Response Class                                    */
/* ************************************************************************** */

class Request;  // Forward declaration

class Response {
public:
	short									StatusCode;
	std::map<std::string, std::string>		Headers;
	std::map<std::string, std::string>		cookies;
	std::string								Body;
	std::string								uri;
	// Streaming support
	bool								StreamFile;
	std::string							FilePath;
	size_t								StreamLength;
	
	Response();
	Response(const Response &Other);
	~Response();
	
	// Build the HTTP response string
	std::string		toString() const;
	
	// Send response to client socket
	void	send(int clientFd) const;
	
	// Helper methods
	void setStatusCode(short code);
	void setHeader(const std::string &key, const std::string &value);
	void setBody(const std::string &body);
	void setStreamFile(const std::string &path, size_t length);

private:
	std::string getStatusMessage(short code) const;
	// Helper to guess basic MIME type by file extension
	static std::string guessContentType(const std::string &path);
	// std::string findMatchingServer(const Request &req, const Config &config) const;
	// std::string findMatchingLocation(const Request &req, const ServerConfig &serverConf) const;
};
