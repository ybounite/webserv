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
	std::string								Body;
	std::string								uri;
	
	Response();
	Response(const Response &Other);
	~Response();
	
	// Static factory method - creates Response from Request + Config
	static Response build(const Request &req, const Config &config);
	
	// Build the HTTP response string
	std::string		toString() const;
	
	// Send response to client socket
	void	send(int clientFd) const;
	
	// Helper methods
	void setStatusCode(short code);
	void setHeader(const std::string &key, const std::string &value);
	void setBody(const std::string &body);

private:
	std::string getStatusMessage(short code) const;
	std::string findMatchingServer(const Request &req, const Config &config) const;
	std::string findMatchingLocation(const Request &req, const ServerConfig &serverConf) const;
};
