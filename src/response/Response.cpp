/* ************************************************************************** */
/*                    file: Response.cpp                                      */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:02 by ybounite                   */
/*                    Updated: 2025/11/29 16:02 by ybounite                   */
/* ************************************************************************** */

# include "Response.hpp"


Response::Response(const Response &Other)
	: Request(Other), StatusCode(Other.StatusCode), Headers(Other.Headers), 
	Fd(Other.Fd), BodySize(Other.BodySize), Header(Other.Header), Body(Other.Body), 
	uri(Other.uri), FilePath(Other.FilePath) {}

Response::~Response() {}

std::string	Response::BuildHeaderResponse() {
	std::ostringstream oss;
	oss << "HTTP/1.1 " << StatusCode << " " << getStatusMessage(StatusCode) << "\r\n";
	oss << "Content-Type: " << guessContentType(FilePath) << "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = Headers.begin(); it != Headers.end(); ++it)
		oss << it->first << ": " << it->second << "\r\n";
	oss << "Content-Length: " << BodySize << "\r\n";

 	if (!cookies.empty()) {
		for (std::map<std::string, std::string>::const_iterator it = cookies.begin(); it != cookies.end(); ++it)
			oss << it->first << ": " << it->second << "\r\n";
	}

	oss << "\r\n";
	return oss.str();
}

void		Response::setStatusCode(short code) {
	StatusCode = code;
}

void		Response::setHeader(const std::string &key, const std::string &value) {
	Headers[key] = value;
}

void		Response::setBody(const std::string &body) {
	Body = body;
	BodySize = body.size();
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

std::string	Response::guessContentType(const std::string &path) {

	std::string::size_type dot = path.find_last_of('.');
	std::string ext = (dot == std::string::npos) ? std::string("") : path.substr(dot + 1);

	if (!_config.MimeTypes.empty()) 
		return _config.MimeTypes[ext];

	return "application/octet-stream";
}
