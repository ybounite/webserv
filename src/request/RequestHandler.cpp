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
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()){
		std::cerr << "Error opening file: " << path << std::endl;
		return "";
	}

	file.seekg(0, std::ios::end);
	long	file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	(void)file_size;
	std::stringstream	buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

bool		isDirectory(const char *path){
	struct stat	st;
	if (stat(path, &st) != 0)	return false;
	return S_ISDIR(st.st_mode);
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
	Response	resp;
	
	// Build file path
	std::string path = config.root + req.getUri();
	if (path[path.length() - 1] == '/') {
		path += config.index;
	}
	//if (path.find(".") == std::string::npos) path += ".html";
	Msg::info("GET: " + path);

	/*Directory handling logic (GET request)

	Typical webserv logic:
	Case									Action
	File exists & readable					Serve file
	Directory + index exists				Serve index
	Directory + autoindex ON				Generate listing
	Directory + no index + autoindex OFF	403
	Path does not exist						404*/

	// Stream file instead of loading entirely into memory
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	// 
	if (!file.is_open()) {
		// File not found
		std::cerr << "Error: unable to open file for reading" << std::endl;
		resp.setStatusCode(404);
		resp.setBody(getErrorPage(404));
	} else {
		std::cout << "open : " << path << std::endl;
		// Determine file size
		file.seekg(0, std::ios::end);
		std::streampos end = file.tellg();
		file.seekg(0, std::ios::beg);
		size_t length = static_cast<size_t>(end);

		resp.setStatusCode(200);
		// Content-Type will be guessed in Response::send if not set
		resp.setHeader("Cache-Control", "no-cache");
		resp.setHeader("Connection", "close");
		resp.setStreamFile(path, length);
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
	else if (contentType.find("multipart/form-data") != std::string::npos)
	{
		std::string boundaryKey = "boundary=";
		size_t pos = contentType.find(boundaryKey);
		if (pos == std::string::npos)
		{
			resp.setStatusCode(400);
			resp.setBody(getErrorPage(400));
			return resp;
		}
		std::string boundary = "--" + contentType.substr(pos + boundaryKey.size());
		std::string body = req.getBody();
		// process each part
		size_t start = 0;
		while ((start = body.find(boundary, start)) != std::string::npos)
		{
			start += boundary.size();
			size_t end = body.find(boundary, start);
			if (end == std::string::npos) break;

			std::string part = body.substr(start, end - start);
			start = end;
			size_t filenamePos = part.find("filename=\"");
			if (filenamePos != std::string::npos)
			{
				filenamePos += 10;
				size_t filenameEnd = part.find("\"", filenamePos);
				std::string filename = part.substr(filenamePos, filenameEnd - filenamePos);

				// Find file content after double \r\n
				size_t contentPos = part.find("\r\n\r\n");
				if (contentPos != std::string::npos)
				{
					std::string fileContent = part.substr(contentPos + 4);
					// Remove trailing \r\n if exists
					// if (fileContent.size() >= 2)
					// 	fileContent.resize(fileContent.size() - 2);

					// Save file
					std::string fullPath = config.locations[1].upload_path + "/" + filename;
					std::ofstream out(fullPath.c_str(), std::ios::binary);
					if (!out.is_open())
					{
						std::cerr << "Failed to open file: " << fullPath << std::endl;
						resp.setStatusCode(500);
						resp.setBody(getErrorPage(500));
						return resp;
					}
					out.write(fileContent.c_str(), fileContent.size());
					out.close();
				}
			}
		}
		resp.setStatusCode(201); // Created
		resp.setHeader("Content-Type", "text/html");
		resp.setBody("<html><body><h1>File uploaded!</h1></body></html>");
	}
	else
	{
		//Process POST data (simplified)
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