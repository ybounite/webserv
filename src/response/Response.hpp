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

class Response : protected Request {
public:

	Response( const Request &req )
		: Request(req), StatusCode(200), Headers(), Fd(-1), BodySize(0), Header(""),
			Body(""), uri(""), FilePath(""), isCGI(false){}
	
	short									StatusCode;
	std::map<std::string, std::string>		Headers;
	int										Fd;
	ssize_t									BodySize;
	std::string								Header;
	std::string								Body;
	std::string								uri;
	std::string								FilePath;
	bool										isCGI;

	
	Response(const Response &Other);
	~Response();
	
	std::string		BuildHeaderResponse();
	void			setStatusCode(short code);
	void			setHeader(const std::string &key, const std::string &value);
	void			setBody(const std::string &body);
	std::string		guessContentType(const std::string &path);
	std::string		getStatusMessage(short code) const;

};
