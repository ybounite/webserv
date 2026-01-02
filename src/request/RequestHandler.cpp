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
	if (!file.is_open()) return 0;

	file.seekg(0, std::ios::end);
	ssize_t file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	file.close();
	if (file_size < 0) return 0;

	return file_size;
}

std::string	RequestHandler::_BuildFileSystemPath(const std::string &root, const std::string &uri)
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

	resp.setStatusCode(code);

	std::ostringstream errorPath;
	errorPath << "errors/" << code << ".html";
	resp.Fd = open(errorPath.str().c_str(), O_RDONLY);
	if (resp.Fd == -1) {
		std::cerr << "Error opening error file: " << errorPath.str() << std::endl;
		// Fallback: set body with inline HTML instead of file
		resp.setHeader("Content-Type", "text/html");
		std::ostringstream fallbackBody;
		fallbackBody << "<html><body><h1>" << code << " " << resp.getStatusMessage(code) << "</h1></body></html>";
		resp.setBody(fallbackBody.str());
		resp.BodySize = fallbackBody.str().size();
	}
	else {
		resp.BodySize = GetBodySize(errorPath.str());
		resp.FilePath = errorPath.str();
	}
	return resp;
}

LocationConfig	GetMatchingLocation(const std::vector<LocationConfig>& locations, const std::string& uri)
{
	LocationConfig* bestMatch = NULL;
	size_t longestMatch = 0;
	//std::cout << "===========================\n";
	for (size_t i = 0; i < locations.size(); i++)
	{
		//std::cout << GREEN << "location : " << locations[i].path << "| root : " << locations[i].root << "| uri : " << uri<< RESET << std::endl;

		const std::string& locPath = locations[i].path;
		if (uri == locPath)
		{
			//std::cout << " loc Path : " << locPath << std::endl;
			if (locPath.size() > longestMatch)
			{ //Example: if uri = "/images/cat.jpg" and locPath = "/images", it matches because /images is at the start.
				bestMatch = const_cast<LocationConfig*>(&locations[i]);
				longestMatch = locPath.size();
			}
		}
	}
	//std::cout << "===========================\n";
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
	if (req.cookies.find("session_id") != req.cookies.end())
	{
		resp.setHeader(
			"Set-Cookie",
			"session_id=" + req.cookies.at("session_id") +
			"; Path=/; HttpOnly; Max-Age=3600"
		);
	}
    return resp;
}

Response	RequestHandler::_GenerateAutoindex(const std::string &DirPath) {
    Response resp(req);
    
    DIR *dir = opendir(DirPath.c_str());
    if (!dir)
        return BuildErrorResponse(500);
    
    std::ostringstream html;
    html << "<html><head><title>Index</title></head><body>";
    html << "<h1>Index of " << req.getUri() << "</h1><ul>";
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name != "." && name != "..")
            html << "<li><a href='" << name << "'>" << name << "</a></li>";
    }
    
    html << "</ul></body></html>";
    closedir(dir);
    
    resp.setStatusCode(200);
    resp.setBody(html.str());
    return resp;
}

bool		RequestHandler::_haseAllowed( std::vector<std::string> Methods, enHttpMethod AllowedMethod)
{
	if (Methods.empty()) return true;
	for (size_t i = 0; i < Methods.size(); i++)
	{
		if ((enHttpMethod)getMethod(Methods[i]) == AllowedMethod)
			return true;
	}
	return false;
}

bool search_Cookies(const std::map<std::string, std::string> &cookies)
{
    std::ifstream file("src/data/data.txt");
    if (!file.is_open())
    {
        std::cerr << "Cannot open data.txt file!" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        for (std::map<std::string,std::string>::const_iterator it = cookies.begin(); it != cookies.end(); ++it)
        {
            std::string cookie_str = it->first + "=" + it->second;
            std::string key = "session_id=";

            size_t pos = line.find(key);
            if (pos == std::string::npos)
                continue;

            std::string sessionPart = line.substr(pos);

            if (sessionPart == cookie_str)
            {
                file.close();
                return true;
            }
        }
    }

    file.close();
    return false;
}

void PrintCookies( std::map<std::string, std::string> header)
{
    std::cout << GREEN << "---- Cookies Map ----" << RESET << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
    std::cout << GREEN << "--------------------" << RESET << std::endl;
}

Response	RequestHandler::handleGET()
{
	LocationConfig loc = GetMatchingLocation(config.locations, req.getUri());
	if (!_haseAllowed(loc.methods, HTTP_GET))
	return BuildErrorResponse(405);
	std::string root = loc.root.empty() ? config.root : loc.root;
	
	std::string	path = _BuildFileSystemPath(root, req.getUri());

	if (!_ResourceExists(path))
	return BuildErrorResponse(404);
	if (_IsDirectory(path.c_str()))
	{
		std::string indexPath = _ResolveIndexFile(path, config, loc);
		std::cout << YELLOW << "indexPath: " << indexPath  << " autoindex : " << loc.autoindex << RESET << std::endl;
    	if (!indexPath.empty()) 
		return serveFile(indexPath);
		else if (loc.autoindex) 
		return _GenerateAutoindex(path);
		else
		return BuildErrorResponse(403);
	}
	Msg::error(path);
	bool isPublicPage = (path == config.root + "/pages/login.html" ||
				path == config.root + "/pages/register.html" || 
					path == config.root + "/pages/index.html" ||
					path.find("/assets/") == 0);

	PrintCookies(req.cookies);
	Msg::success(path);
	std::cout << RED << search_Cookies(req.cookies) << RESET << std::endl;
	std::cout << RED << isPublicPage << RESET << std::endl;

 if (!isPublicPage && !search_Cookies(req.cookies))
    {
        Response resp(req);
        resp.setStatusCode(302);
        resp.setHeader("Location", "/pages/login.html");
        resp.setBody("<html><body>Redirecting to login...</body></html>");
        return resp;
    }

    Response resp = serveFile(path);

    if (req.cookies.find("session_id") != req.cookies.end())
    {
        resp.setHeader(
            "Set-Cookie",
            "session_id=" + req.cookies.at("session_id") +
            "; Path=/; HttpOnly; Max-Age=3600"
        );
    }

    return resp;
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
	if (req.status != Request::enVALID)
		return BuildErrorResponse(400);
	switch (getMethod(req.getMethod()))
	{
	case HTTP_GET:
		return handleGET();
	case HTTP_POST:
		return handlePOST();
	case HTTP_DELETE:
		return handleDELETE();
	default:
		return BuildErrorResponse(405);
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
	return ""; 
}

std::string clean(const std::string &str)
{
    std::string s = str;
    while (!s.empty() && (s[s.size()-1] == '\n' || s[s.size()-1] == '\r'))
        s.erase(s.size()-1);
    return s;
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

		// for (std::map<std::string,std::string>::iterator it = form.begin(); it != form.end(); ++it)
		// 	std::cout << "FORM: [" << it->first << "] = [" << it->second << "]" << std::endl;

		std::string username = form.count("username") ? form["username"] : "";
		std::string password = form.count("password") ? form["password"] : "";

		if (!username.empty() && username[username.size()-1] == '\r')
			username.erase(username.size()-1);
		if (!password.empty() && password[password.size()-1] == '\r')
			password.erase(password.size()-1);

		bool loginSuccess = false;

		if (req.getUri() == "/pages/register.html")
		{
			std::ofstream outfile("src/data/data.txt", std::ios::app);
			outfile << std::endl;
			if (!outfile.is_open())
				throw "Cannot open data.txt file!";

			std::string sessionId = "none";
			if (!req.cookies.empty())
				sessionId = "session_id=" + req.cookies.begin()->second;

			outfile << username << " " << password << " " << sessionId << std::endl;
		}

		if (req.getUri() == "/pages/login.html")
		{
			std::ifstream infile("src/data/data.txt");
			if (!infile.is_open())
				throw "Cannot open data.txt for login check!";

			std::vector<std::string> lines;
			std::string line;
			bool loginUpdated = false;

			std::string sessionId = "session_id=" + req.cookies.begin()->second;

			while (std::getline(infile, line))
			{
				if (line.empty()) {
					lines.push_back(line);
					continue;
				}

				std::istringstream iss(line);
				std::string fileUser, filePass, fileSession;
				if (!(iss >> fileUser >> filePass >> fileSession)) {
					lines.push_back(line);
					continue; // keep malformed lines as is
				}

				if (fileUser == username && filePass == password)
				{
					// update the session with the one from request
					line = fileUser + " " + filePass + " " + sessionId;
					loginSuccess = true;
					loginUpdated = true;
				}

				lines.push_back(line);
			}
			infile.close();

			if (loginUpdated)
			{
				std::ofstream outfile("src/data/data.txt", std::ios::trunc);
				if (!outfile.is_open())
					throw "Cannot open data.txt to update session!";

				for (size_t i = 0; i < lines.size(); ++i)
				{
					outfile << lines[i];
					if (i + 1 < lines.size()) outfile << "\n";
				}
			}
		}

		std::string html;

		if (req.getUri() == "/pages/login.html" && !loginSuccess)
		{
			html =
			"<html><head>"
			"<meta charset='UTF-8'>"
			"<script src='https://cdn.tailwindcss.com'></script>"
			"</head><body class='bg-gray-100 flex items-center justify-center h-screen'>"
			"<div class='fixed inset-0 flex items-center justify-center bg-black bg-opacity-50'>"
			"<div class='bg-white rounded-xl shadow-lg p-6 text-center w-80'>"
			"<h2 class='text-xl font-bold text-red-600 mb-4'>Login Failed ❌</h2>"
			"<p class='text-gray-700 mb-6'>Invalid username or password</p>"
			"<button onclick=\"window.location.href='/pages/login.html'\" "
			"class='bg-red-500 hover:bg-red-600 text-white px-4 py-2 rounded-lg w-full transition'>Try Again</button>"
			"</div></div></body></html>";
		}

		else
		{
			html =
			"<html><head>"
			"<meta charset='UTF-8'>"
			"<script src='https://cdn.tailwindcss.com'></script>"
			"</head><body class='bg-gray-100 flex items-center justify-center h-screen'>"
			"<div class='fixed inset-0 flex items-center justify-center bg-black bg-opacity-50'>"
			"<div class='bg-white rounded-xl shadow-lg p-6 text-center w-80'>"
			"<h2 class='text-xl font-bold text-green-600 mb-4'>Success 🎉</h2>"
			"<p class='text-gray-700 mb-6'>Welcome!</p>"
			"<button onclick=\"window.location.href='/pages/home.html'\" "
			"class='bg-green-500 hover:bg-green-600 text-white px-4 py-2 rounded-lg w-full transition'>OK</button>"
			"</div></div></body></html>";
		}

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

	LocationConfig loc = GetMatchingLocation(config.locations, req.getUri());
	// if (!_haseAllowed(loc.methods, HTTP_GET))
	// 	return BuildErrorResponse(405);
	std::string root = loc.root.empty() ? config.root : loc.root;
	std::string	path = _BuildFileSystemPath(root, req.getUri());
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return BuildErrorResponse(404);

	if (std::remove(path.c_str()) != 0)
		return BuildErrorResponse(500);

	resp.setStatusCode(204);
	// resp.setBody("");
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

