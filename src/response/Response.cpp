/* ************************************************************************** */
/*                    file: Response.cpp                                      */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:02 by ybounite                   */
/*                    Updated: 2025/11/29 16:02 by ybounite                   */
/* ************************************************************************** */

# include "Response.hpp"

Response::Response() : StatusCode(200), StreamFile(false), FilePath(""), StreamLength(0) {}

Response::Response(const Response &Other) {

	StatusCode = Other.StatusCode;
	Headers = Other.Headers;
	Body = Other.Body;
	uri = Other.uri;
}

Response::~Response() {}

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
	if (!StreamFile) {
		// Default non-streaming path
		std::string responseStr = toString();
		::send(clientFd, responseStr.c_str(), responseStr.length(), 0);
		return;
	}

	// Streaming path: build and send headers first
	std::ostringstream oss;
	oss << "HTTP/1.1 " << StatusCode << " " << getStatusMessage(StatusCode) << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = Headers.begin(); it != Headers.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
		// std::cout << RED << it->first << "-------#----" << it->second  << RESET<< "\n";
	}
	for (std::map<std::string, std::string>::const_iterator it = cookies.begin(); it != cookies.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
	}

	// Ensure Content-Type present
	if (Headers.find("Content-Type") == Headers.end()) {
		oss << "Content-Type: " << guessContentType(FilePath) << "\r\n";
	}
	// Use Content-Length with known file size
	oss << "Content-Length: " << StreamLength << "\r\n";
	// Connection header default
	if (Headers.find("Connection") == Headers.end()) {
		oss << "Connection: close\r\n";
	}
	// End headers
	oss << "\r\n";

	std::string headerStr = oss.str();
	::send(clientFd, headerStr.c_str(), headerStr.length(), 0);

	// Stream file body in chunks
	std::ifstream file(FilePath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		// Fallback error body if file can't be opened
		std::string err = "Failed to open file";
		::send(clientFd, err.c_str(), err.length(), 0);
		return;
	}

	const std::size_t CHUNK_SIZE = 64 * 1024; // 64KB
	char buffer[CHUNK_SIZE];
	while (file) {
		file.read(buffer, CHUNK_SIZE);
		std::streamsize count = file.gcount();
		if (count > 0) {
			const char *ptr = buffer;
			std::streamsize remaining = count;
			// Attempt to write the chunk; handle short writes
			while (remaining > 0) {
				ssize_t sent = ::send(clientFd, ptr, static_cast<size_t>(remaining), 0);
				if (sent <= 0) {
					// Stop on error; in real server handle EAGAIN/EWOULDBLOCK
					return;
				}
				ptr += sent;
				remaining -= sent;
			}
		}
	}
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

// Basic content-type guessing
std::string Response::guessContentType(const std::string &path) {
	std::string::size_type dot = path.find_last_of('.');
	std::string ext = (dot == std::string::npos) ? std::string("") : path.substr(dot + 1);
	if (ext == "html" || ext == "htm") return "text/html";
	if (ext == "css") return "text/css";
	if (ext == "js") return "application/javascript";
	if (ext == "json") return "application/json";
	if (ext == "png") return "image/png";
	if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
	//if (ext == "gif") return "image/gif";
	//if (ext == "svg") return "image/svg+xml";
	//if (ext == "ico") return "image/x-icon";
	if (ext == "pdf") return "application/pdf";
	if (ext == "mp4") return "video/mp4";
	return "application/octet-stream";
}

void Response::setStreamFile(const std::string &path, size_t length) {
	StreamFile = true;
	FilePath = path;
	StreamLength = length;
}
