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

class Response {
public:

	short					StatusCode;
	std::map<std::string, std::string>	Headers;
	std::string				Body;
	std::string				uri;
	Response( void );
	Response( const  Response &Other );
	~Response( void );
	//Response    handle(const Request &req, const ServerConfig &config);


private:
};
