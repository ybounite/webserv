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

	Response( const Request &req )
		: StatusCode(200), Headers(), Fd(-1), BodySize(0), Header(""), Body(""), uri(""),
		  StreamFile(false), FilePath(""), StreamLength(0), _req(req) {}

	short									StatusCode;
	std::map<std::string, std::string>		Headers;
	int										Fd;
	ssize_t									BodySize;
	std::string								Header;
	std::string								Body;
	std::string								uri;
	// Streaming support
	bool								StreamFile;
	std::string							FilePath;
	size_t								StreamLength;
	
	Response(const Response &Other);
	~Response();
	
	// Send response to client socket
	//void	send(int clientFd) const;
	std::string		BuildHeaderResponse() const;
	void			setStatusCode(short code);
	void			setHeader(const std::string &key, const std::string &value);
	void			setBody(const std::string &body);
	void			setStreamFile(const std::string &path, size_t length);
	static std::string guessContentType(const std::string &path);
	std::string getStatusMessage(short code) const;

private:
	const Request &_req;
};
