/* ************************************************************************** */
/*                    file: Response.cpp                                      */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:02 by ybounite                   */
/*                    Updated: 2025/11/29 16:02 by ybounite                   */
/* ************************************************************************** */

# include "Response.hpp"

Response::Response() : StatusCode(200) {}

Response::Response(const Response &Other) {

	StatusCode = Other.StatusCode;
	Headers = Other.Headers;
	Body = Other.Body;
	uri = Other.uri;
}

Response::~Response() {}

// Static factory method - This is where you process Request + Config
Response	Response::build(const Request &req, const Config &config) {
	Response	resp;

	// 1. Get request details
	std::string	method = req.getMethod();
	std::string	uri = req.getUri();
	resp.uri = uri;

	// 2. Find matching server from config
	// (You'll need to implement logic to match server based on Host header, port, etc.)
	if (config.servers.empty()) {
		resp.setStatusCode(500);
		resp.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
		return resp;
	}

	const ServerConfig &serverConf = config.servers[0]; // Simplified - should match properly

	// 3. Find matching location
	// (You'll need to implement location matching based on URI)

	// 4. Handle different methods
	if (method == "GET") {
		// Handle GET request
		// Read file, return content
		resp.setStatusCode(200);
		resp.setHeader("Content-Type", "text/html");
		resp.setBody("<html><body><h1>Hello from Response!</h1></body></html>");
	}
	else if (method == "POST") {
		// Handle POST request
		// Save uploaded data
		resp.setStatusCode(201);
		resp.setBody("<html><body><h1>Created</h1></body></html>");
	}
	else if (method == "DELETE") {
		// Handle DELETE request
		resp.setStatusCode(204);
		resp.setBody("");
	}
	else {
		// Method not allowed
		resp.setStatusCode(405);
		resp.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
	}
	(void)serverConf; // Suppress unused warning for now
	return resp;
}

// Build the complete HTTP response string
std::string Response::toString() const {
	std::ostringstream oss;
		// Status line
	oss << "HTTP/1.1 " << StatusCode << " " << getStatusMessage(StatusCode) << "\r\n";
		// Headers
	for (std::map<std::string, std::string>::const_iterator it = Headers.begin(); 
		it != Headers.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
	}
		// Content-Length header
	oss << "Content-Length: " << Body.length() << "\r\n";
		// Empty line separating headers from body
	oss << "\r\n";
		// Body
	oss << Body;
	return oss.str();
}

// Send response to client
void		Response::send(int clientFd) const {
	std::string responseStr = toString();
	::send(clientFd, responseStr.c_str(), responseStr.length(), 0);
}

void		Response::setStatusCode(short code) {
	StatusCode = code;
}

void		Response::setHeader(const std::string &key, const std::string &value) {
	Headers[key] = value;
}

void		Response::setBody(const std::string &body) {
Body = body;
}

std::string	Response::getStatusMessage(short code) const {
	switch (code) {
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 500: return "Internal Server Error";
		case 505: return "HTTP Version Not Supported";
		default: return "Unknown";
	}
}
