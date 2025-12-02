/* ************************************************************************** */
/*                    file: RequestHandler.cpp                                */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:09 by ybounite                   */
/*                    Updated: 2025/11/29 16:09 by ybounite                   */
/* ************************************************************************** */

# include "RequestHandler.hpp"

RequestHandler::RequestHandler( void ) {}

RequestHandler::RequestHandler( const Request &Other ) : Request(Other) {}

RequestHandler::RequestHandler( const RequestHandler &Other ){
	*this = Other;
}

RequestHandler::~RequestHandler( void ) {}

std::string	readFile(const std::string &path) {

	std::ifstream file(path.c_str());
	if (!file.is_open()) return "";

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string	getStatusText( int statusCode ) {
	switch (statusCode) {
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 413: return "Payload Too Large";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		default:  return "Unknown";
	}
}

std::string	RequestHandler::buildHttpResponse(int statusCode, const std::string &body) {

	const std::string statusText = getStatusText(statusCode);

	std::ostringstream response;
	// Status line: "HTTP/1.1 200 OK"
	response << getHTTPversion() << " " << statusCode << " " << statusText << "\r\n";

	response << getHTTPversion() << statusCode << " " << statusText << "\r\n";
	response << "Content-Length: " << body.size() << "\r\n";
	response << "Content-Type: text/html\r\n";
	response << "\r\n"; // blank line before body
	response << body;

	return response.str();
}

std::string     RequestHandler::handle(const Config &config) {
	// Response     res;
	std::cout << "Method is : " << this->getMethod() << std::endl;
	int 		status;
	std::string	body;
	std::string Path = config.servers[0].root + this->getUri();
	if (Path[Path.length() - 1] == '/')
			Path += config.servers[0].index;
	std::cout << GREEN << Path << RESET << std::endl;
// struct       stat st;
// if (stat(Path.c_str(), &st) == -1) 
//      std::cout << RED <<  "file is not fond !" << RESET << std::endl;
	if (getMethod() == "GET") {
		status = 200;
		body = readFile(Path);
	}else if (getMethod() == "POST"){
		status = 200;
		// Handler POST
		// std::string body = readFile(Path);
	}else if (getMethod() == "DELETE") {
		// Handler DELETE
		status = 200;
	}else {
		status = 405;
		body = readFile("errors/405.html");
		// body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
	}

	if (body.empty()) {
		status = 404;
		body = readFile("errors/404.html");
		// body = "<html><body><h1>404 Not Found</h1></body></html>";
	}
	// Response	response(status, body);
	return  buildHttpResponse(status, body);
}