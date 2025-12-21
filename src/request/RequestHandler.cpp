/* ************************************************************************** */
/*                    file: RequestHandler.cpp                                */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:09 by ybounite                   */
/*                    Updated: 2025/11/29 16:09 by ybounite                   */
/* ************************************************************************** */

# include "RequestHandler.hpp"
# include "Request.hpp"
# include "../response/Response.hpp"

std::string	RequestHandler::readFile(const std::string &path) {
	std::ifstream file(path.c_str());
	if (!file.is_open()) return "";

	std::stringstream	buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string	RequestHandler::getErrorPage(int statusCode) {
	std::ostringstream errorPath;
	errorPath << "errors/" << statusCode << ".html";
	std::string content = readFile(errorPath.str());
	
	if (content.empty()) {
		// Fallback if error page doesn't exist
		std::ostringstream fallback;
		fallback << "<html><body><h1>" << statusCode << " Error</h1></body></html>";
		return fallback.str();
	}
	return content;
}

short		RequestHandler::getMothod( const std::string &method ) {
	if (method == "GET")
		return HTTP_GET;
	else if (method == "POST")
		return HTTP_POST;
	else if (method == "DELETE")
		return HTTP_DELETE;
	else
		return HTTP_UNKNOWN;
}

// Main handler - routes to appropriate method handler
Response	RequestHandler::handle(const Request &req, const Config &config) {

	if (config.servers.empty()) {
		Response	resp;
		resp.setStatusCode(500);
		resp.setBody(getErrorPage(500));
		return resp;
	}
	
	// Get the first server config (simplified - should match by host/port)
	const ServerConfig &serverConf = config.servers[0];
	
	// Route based on method
	//std::string method = req.getMethod();
	switch (getMothod(req.getMethod())) {
	case HTTP_GET:
		return handleGET(req, serverConf);
	case HTTP_POST:
		return handlePOST(req, serverConf);
	case HTTP_DELETE:
		return handleDELETE(req, serverConf);
	default :
		Response resp;
		resp.setStatusCode(405);
		resp.setBody(getErrorPage(405));
		return resp;
	}
}

Response	RequestHandler::handleGET(const Request &req, const ServerConfig &config) {
	Response resp;
	
	// Build file path
	std::string path = config.root + req.getUri();
	if (path[path.length() - 1] == '/') {
		path += config.index;
	}
	
	Msg::info("GET: " + path);
	
	// Read file
	std::string body = readFile(path);
	
	if (body.empty()) {
		// File not found
		resp.setStatusCode(404);
		resp.setBody(getErrorPage(404));
	} else {
		// Success
		resp.setStatusCode(200);
		resp.setHeader("Content-Type", "text/html");
		resp.setBody(body);
	}
	
	return resp;
}

/////////////////////
std::map<std::string, std::string> parseUrlEncoded(const std::string &body)
{
	std::map<std::string, std::string> data;
	std::stringstream ss(body);
	std::string pair;

	while (std::getline(ss, pair, '&')) {
		size_t pos = pair.find('=');
		if (pos != std::string::npos) {
			std::string key = pair.substr(0, pos);
			std::string value = pair.substr(pos + 1);
			data[key] = value;
		}
	}
	return data;
}
////////////////////

Response	RequestHandler::handlePOST(const Request &req, const ServerConfig &config) {
	Response resp;
	(void)config; // Unused for now
	
	std::string bodyData = req.getBody();
	std::string contentType = req.getHeader("Content-Type");
	
	if (bodyData.empty()) {
		resp.setStatusCode(400);
		resp.setBody(getErrorPage(400));
	}
	else if (bodyData.size() > config.client_max_body_size)
	{
		resp.setStatusCode(413);
		return resp;
	}

	else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		std::map<std::string, std::string> form = parseUrlEncoded(bodyData);

		std::string html = "<html><body><h1>Form Data</h1><ul>";
		for (std::map<std::string, std::string>::iterator it = form.begin(); it != form.end(); ++it)
			html += "<li>" + it->first + " = " + it->second + "</li>";
		html += "</ul></body></html>";

		resp.setStatusCode(200);
		resp.setHeader("Content-Type", "text/html");
		resp.setBody(html);
	}
	else
	{
		// Process POST data (simplified)
		resp.setStatusCode(200);
		resp.setHeader("Content-Type", "text/html");
		
		std::string responseBody = 
			"<html><body>"
			"<h1>POST received</h1>"
			"<pre>" + bodyData + "</pre>"
			"</body></html>";
		resp.setBody(responseBody);
	}
	
	return resp;
}

Response	RequestHandler::handleDELETE(const Request &req, const ServerConfig &config) {
	Response resp;
	(void)req;
	(void)config;
	
	// TODO: Implement file deletion
	resp.setStatusCode(204); // No Content
	resp.setBody("");
	
	return resp;
}