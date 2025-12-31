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

size_t RequestHandler::GetBodySize(const std::string &path) {
	std::ifstream file(path.c_str(), std::ios::binary);
	//if (!file.is_open()) {
	//	std::cerr << "Error opening file: " << path << std::endl;
	//	return 0;
	//}
	file.seekg(0, std::ios::end);
	long file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	file.close();
	if (file_size < 0)
		return 0;
	return file_size;
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

Response	RequestHandler::BuildErrorResponse( short code ) {

	Response resp(req);
	std::ostringstream errorPath;
	errorPath << "errors/" << code << ".html";
	resp.Fd = open(errorPath.str().c_str(), O_RDONLY);
	if (resp.Fd == -1) {
		std::cerr << "Error opening error file: " << errorPath.str() << std::endl;
		// Fallback: set body with inline HTML instead of file
		std::ostringstream fallbackBody;
		fallbackBody << "<html><body><h1>" << code << " " << resp.getStatusMessage(code) << "</h1></body></html>";
		resp.setBody(fallbackBody.str());
		resp.BodySize = fallbackBody.str().size();
	} else {
		resp.BodySize = GetBodySize(errorPath.str());
	}
	resp.FilePath = errorPath.str();
	resp.setStatusCode(code);
	return resp;
}

LocationConfig	GetMatchingLocation(const std::vector<LocationConfig>& locations, const std::string& uri)
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
			}
		}
	}

	if (bestMatch)
		return *bestMatch;
	return LocationConfig();
}

Response	RequestHandler::serveFile(const std::string &path)
{
    Response resp(req);
	
	resp.Fd = open(path.c_str(), O_RDONLY);
	if (resp.Fd == -1)
        return BuildErrorResponse(404);
	resp.BodySize = GetBodySize(path);
    resp.setStatusCode(200);
	resp.FilePath = path;
    return resp;
}

Response	RequestHandler::handleGET()
{
	LocationConfig loc = GetMatchingLocation(config.locations, req.getUri());

	std::string root = loc.root.empty() ? config.root : loc.root;
	//std::cout << RED << "location : " << loc.root << RESET << std::endl;
	//std::cout << GREEN << "root : " << config.root << RESET << std::endl;
	//std::cout << YELLOW << "URI : " << req.getUri() << RESET << std::endl;

	std::string	path = _BuildFileSystemPath(root, req.getUri());
	if (!_ResourceExists(path))
		return BuildErrorResponse(404);
	//std::cout << "*** : " << path << ": ***" << std::endl;
	if (_IsDirectory(path.c_str()))
	{
		std::cout << "Yes directory \n";
		std::string indexPath = _ResolveIndexFile(path, config, loc);
		//std::cout << "indexPath: " << indexPath  << " autoindex : " << loc.autoindex << std::endl;
		if (!indexPath.empty()) {
			return serveFile(indexPath);
		}
		if (!loc.autoindex)
			return BuildErrorResponse(403);
		return BuildErrorResponse(403);
	}
	//std::cout << "*** : " << path << ": ***" << std::endl;
	return serveFile(path);
}

short		RequestHandler::getMethod(const std::string &method)
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

Response	RequestHandler::HandleMethod()
{
	Response resp(req);
	switch (getMethod(req.getMethod()))
	{
	case HTTP_GET:
		return handleGET();
	case HTTP_POST:
		return handlePOST();
	case HTTP_DELETE:
		return handleDELETE();
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

Response RequestHandler::handlePOST()
{
	Response resp(req);
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

Response RequestHandler::handleDELETE()
{
	Response resp(req);

	resp.setStatusCode(204);
	resp.setBody("");

	return resp;
}

std::string RequestHandler::readFile(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return "";
	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string RequestHandler::getErrorPage(int statusCode)
{
	std::map<int, std::string>::const_iterator it = config.error_pages.find(statusCode);
	std::string path;
	if (it != config.error_pages.end())
		path = it->second;
	else
	{
		std::ostringstream oss;
		oss << "errors/" << statusCode << ".html";
		path = oss.str();
	}

	std::string content = readFile(path);
	if (!content.empty())
		return content;

	std::ostringstream fallback;
	fallback << "<html><body><h1>" << statusCode << " " << statusCode << "</h1></body></html>";
	return fallback.str();
}