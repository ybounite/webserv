/* ************************************************************************** */
/*                    file: RequestHandler.cpp                                */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:09 by ybounite                   */
/*                    Updated: 2025/11/29 16:09 by ybounite                   */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "Request.hpp"
#include "../response/Response.hpp"

std::string RequestHandler::readFile(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Error opening file: " << path << std::endl;
		return "";
	}

	file.seekg(0, std::ios::end);
	long file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	(void)file_size;
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string RequestHandler::_BuildFileSystemPath(const std::string &root, const std::string &uri)
{
    std::string path = root;
    if (!path.empty() && path[path.size() -1 ] != '/')
        path += "/";
    path += (uri[0] == '/' ? uri.substr(1) : uri); // avoid double slash
    return path;
}

bool		RequestHandler::_ResourceExists( std::string &Path ){
	struct stat Buffer;
	return (stat(Path.c_str(), &Buffer) == 0);
}

std::string RequestHandler::_ResolveIndexFile(const std::string &path, const ServerConfig &server, const LocationConfig &loc)
{
    std::string indexName = !loc.index.empty() ? loc.index : server.index;
    if (indexName.empty())
        return "";

    std::string fullPath = path;
    if (fullPath[fullPath.size() - 1] != '/')
        fullPath += '/';
    fullPath += indexName;
    if (_ResourceExists(fullPath))
        return fullPath;

    return "";
}

bool	_IsDirectory(const char *path)
{
	struct stat st;
	if (stat(path, &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}

std::string RequestHandler::getErrorPage(int statusCode)
{
	std::ostringstream errorPath;
	errorPath << "errors/" << statusCode << ".html";
	std::string content = readFile(errorPath.str());

	if (content.empty())
	{
		// Fallback if error page doesn't exist
		std::ostringstream fallback;
		fallback << "<html><body><h1>" << statusCode << " Error</h1></body></html>";
		return fallback.str();
	}
	return content;
}

Response	RequestHandler::BuildErrorResponse( short code ) {

	Response resp;
	resp.setStatusCode(code);
	resp.setBody(getErrorPage(code));
	return resp;
}

LocationConfig GetMatchingLocation(const std::vector<LocationConfig>& locations, const std::string& uri)
{
	LocationConfig* bestMatch = NULL;
	size_t longestMatch = 0;

	for (size_t i = 0; i < locations.size(); i++)
	{
		const std::string& locPath = locations[i].path;
		if (uri.find(locPath) == 0)
		{
			if (locPath.size() > longestMatch)
			{ //Example: if uri = "/images/cat.jpg" and locPath = "/images", it matches because /images is at the start.
				bestMatch = const_cast<LocationConfig*>(&locations[i]);
				longestMatch = locPath.size();
				std::cout << "Locations [" << i << "] -> " << locations[i].path << std::endl;
			}
		}
	}

	if (bestMatch)
		return *bestMatch;
	return LocationConfig();
}

Response	RequestHandler::serveFile(const std::string &path)
{
    Response resp;

    std::string content = readFile(path);
    if (content.empty())
        return BuildErrorResponse(404);

    resp.setStatusCode(200);
    resp.setBody(content);

    // Optional: set MIME type (simplified example)
    if (path.find(".html") != std::string::npos)
        resp.setHeader("Content-Type", "text/html");
    else if (path.find(".css") != std::string::npos)
        resp.setHeader("Content-Type", "text/css");
    else if (path.find(".js") != std::string::npos)
        resp.setHeader("Content-Type", "application/javascript");
    else if (path.find(".png") != std::string::npos)
        resp.setHeader("Content-Type", "image/png");
    else if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
        resp.setHeader("Content-Type", "image/jpeg");
    else
        resp.setHeader("Content-Type", "application/octet-stream");

    return resp;
}

Response RequestHandler::generateAutoindexResponse(const std::string &dirPath,
			const std::string &uri) {
    Response resp;
    std::ostringstream html;

    html << "<html><head><title>Index of " << uri << "</title></head><body>";
    html << "<h1>Index of " << uri << "</h1><ul>";

    DIR *dir = opendir(dirPath.c_str());
    if (!dir)
        return BuildErrorResponse(403);  // Cannot open directory

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        // Skip current and parent directories
        if (name == "." || name == "..")
            continue;

        // Build link
        html << "<li><a href=\"" << uri;
        if (uri[uri.size() - 1] != '/')
            html << '/';
        html << name << "\">" << name << "</a></li>";
    }

    closedir(dir);

    html << "</ul></body></html>";

    resp.setStatusCode(200);
    resp.setBody(html.str());
    resp.setHeader("Content-Type", "text/html");

    return resp;
}

Response RequestHandler::handleGET(const Request &req, const ServerConfig &server)
{
	Response resp;
	//system("clear");
	LocationConfig loc = GetMatchingLocation(server.locations, req.getUri());

	std::string root = loc.root.empty() ? server.root : loc.root;
	std::cout << RED << "location : " << loc.root << RESET << std::endl;
	std::cout << GREEN << "root : " << server.root << RESET << std::endl;
	std::cout << YELLOW << "URI : " << req.getUri() << RESET << std::endl;

	std::string path = _BuildFileSystemPath(root, req.getUri());
	std::cout << "*** : " << path << ": ***" << std::endl;
	if (!_ResourceExists(path))
		return BuildErrorResponse(404);

	if (_IsDirectory(path.c_str()))
	{
		std::cout << "Yes directory \n";
		std::string indexPath = _ResolveIndexFile(path, server, loc);
		//std::cout << "indexPath: " << indexPath  << " autoindex : " << loc.autoindex << std::endl;
		if (!loc.autoindex)
			return BuildErrorResponse(403);
		if (!indexPath.empty()) {
			return serveFile(indexPath);
		}
	}
	else 
		std::cout << "### is file " << path << " ###" << std::endl;
	
	return serveFile(path);
}


short		RequestHandler::getMothod(const std::string &method)
{
	if (method == "GET")
		return HTTP_GET;
	else if (method == "POST")
		return HTTP_POST;
	else if (method == "DELETE")
		return HTTP_DELETE;
	else
		return HTTP_UNKNOWN;
}

Response	RequestHandler::handle(const Request &req, const Config &config)
{
	Response resp;
	//server = config.servers[0];
	const ServerConfig &serverConf = config.servers[0];
	switch (getMothod(req.getMethod()))
	{
	case HTTP_GET:
		return handleGET(req, serverConf);
	case HTTP_POST:
		return handlePOST(req, serverConf);
	case HTTP_DELETE:
		return handleDELETE(req, serverConf);
	default:
		resp.setStatusCode(405);
		resp.setBody(getErrorPage(405));
		return resp;
	}
}

/////////////////////
std::map<std::string, std::string> parseUrlEncoded(const std::string &body)
{
	std::map<std::string, std::string> data;
	std::stringstream ss(body);
	std::string pair;

	while (std::getline(ss, pair, '&'))
	{
		size_t pos = pair.find('=');
		if (pos != std::string::npos)
		{
			std::string key = pair.substr(0, pos);
			std::string value = pair.substr(pos + 1);
			data[key] = value;
		}
	}
	return data;
}

std::string getUploadPath(const ServerConfig &config)
{
	for (size_t i = 0; i < config.locations.size(); ++i)
	{
		if (!config.locations[i].upload_path.empty())
			return config.locations[i].upload_path;
	}
	return ""; // none found
}

////////////////////

Response RequestHandler::handlePOST(const Request &req, const ServerConfig &config)
{
	Response resp;
	std::string bodyData = req.getBody();
	std::string contentType = req.getHeader("Content-Type");
	if (bodyData.empty())
	{
		resp.setStatusCode(400);
		resp.setBody(getErrorPage(400));
	}
	else if (bodyData.size() > config.client_max_body_size)
	{
		resp.setStatusCode(413);
		resp.setBody(getErrorPage(413));
		return resp;
	}

	else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		std::map<std::string, std::string> form = parseUrlEncoded(bodyData);

		std::string html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Form</title><link rel='stylesheet' href='/assets/css/main.css'></head><body><h1>Form Data</h1><ul></ul></body></html>";
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
			if (end == std::string::npos)
				break;

			std::string part = body.substr(start, end - start);
			start = end;
			size_t filenamePos = part.find("filename=\"");
			if (filenamePos != std::string::npos)
			{
				filenamePos += 10;
				size_t filenameEnd = part.find("\"", filenamePos);
				std::string filename = part.substr(filenamePos, filenameEnd - filenamePos);
				size_t contentPos = part.find("\r\n\r\n");
				if (contentPos != std::string::npos)
				{
					std::string fileContent = part.substr(contentPos + 4);
					if (fileContent.size() >= 2)
						fileContent.resize(fileContent.size() - 2);
					std::string uploadDir = getUploadPath(config);
					if (uploadDir.empty())
					{
						resp.setStatusCode(500);
						resp.setBody(getErrorPage(500));
						return resp;
					}
					std::string fullPath = uploadDir + "/" + filename;
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
		resp.setStatusCode(201);
		resp.setHeader("Content-Type", "text/html");
		resp.setBody(readFile("www/uploaded.html"));
	}
	else
	{
		resp.setStatusCode(200);
		resp.setHeader("Content-Type", "text/html");

		std::string responseBody = "<html>"
		"<head>"
		"<meta charset='UTF-8'>"
		"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
		"<title>POST Result</title>"
		"<link rel='stylesheet' href='/assets/css/main.css'>"
		"</head>"
		"<body>"
		"<h1>POST received</h1>"
		"<pre>" + bodyData + "</pre>"
		"</body>"
		"</html>";

		resp.setBody(responseBody);
	}

	return resp;
}

Response RequestHandler::handleDELETE(const Request &req, const ServerConfig &config)
{
	Response resp;
	(void)req;
	(void)config;

	resp.setStatusCode(204);
	resp.setBody("");

	return resp;
}