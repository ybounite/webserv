/* ************************************************************************** */
/*                    file: Request.cpp                                       */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/16 09:42 by ybounite                   */
/*                    Updated: 2025/11/16 09:42 by ybounite                   */
/* ************************************************************************** */

#include "Request.hpp"
// # include "RequestHandler.hpp"

Request::Request(void) {}

Request::Request(std::string raw, Config &ConfigFile) : _config(ConfigFile)
{
	this->handleRequest(raw);
}

Request::Request(const Request &Other)
{
	*this = Other;
}

Request &Request::operator=(const Request &Other)
{
	if (this != &Other)
	{
		_Method = Other._Method;
		_URI = Other._URI;
		_HTTPversion = Other._HTTPversion;
		_Headers = Other._Headers;
		_Body = Other._Body;
		_Path = Other._Path;
		_config = Other._config;
	}
	return *this;
}

Request::~Request(void) {}

const std::string &Request::getMethod(void) const { return _Method; }
const std::string &Request::getUri(void) const { return _URI; }
const std::string &Request::getHTTPversion(void) const { return _HTTPversion; }
const std::map<std::string, std::string> &Request::getHeaders(void) const { return _Headers; }
const std::string &Request::getBody(void) const { return _Body; }
const std::string &Request::getPath(void) const { return _Path; }

void Request::parseRequestLine(const std::string &line)
{
	std::istringstream ss(line);
	ss >> _Method >> _URI >> _HTTPversion;
	// std::cout << GREEN << "**********************" << RESET << std::endl;
	// std::cout << "* Method : " << _Method << std::endl;
	// std::cout << "* URI : " << _URI << std::endl;
	// std::cout << "* HTTPversion : " << _HTTPversion << std::endl;
	// std::cout << GREEN << "**********************" << RESET << std::endl;
}

void Request::parseHeaders(std::istringstream &stream)
{
	std::cout << GREEN << "Hendling HTTP request" << RESET << std::endl;
	std::string Line;
	while (getline(stream, Line))
	{

		if (!Line.empty() && Line[Line.length() - 1] == '\r') // this '\r' trailing
			Line.erase(Line.length() - 1);

		if (Line.empty())
			break;
		size_t pos = Line.find(":");
		if (pos == std::string::npos)
			throw std::runtime_error("invalid header field");

		std::string key = Line.substr(0, pos);
		std::string value = Line.substr(pos + 1);

		if (!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		// std::cout << "this key :" << key << std::endl;
		// std::cout << "this value :" << value << std::endl;
		this->_Headers[key] = value;
	}
}

void printRequest(std::string &row)
{
	std::cout << YELLOW << "****************Request************" << RESET << std::endl;
	std::cout << row << std::endl;
	std::cout << YELLOW << "****************END****************" << RESET << std::endl;
}

/*
		* ⚠ Important limitations
	✔ This function only works for Content-Length, not for:
	✔ Transfer-Encoding: chunked
	✔ multipart form decoding
	✔ file uploads (multipart/form-data)
	✔ I can show you how to add support for those too.
*/

void Request::parseBody(std::istringstream &stream)
{
	std::string lenStr = _Headers["Content-Length"];
	if (lenStr.empty())
		return;

	size_t len = std::atoi(lenStr.c_str());
	if (len == 0)
		return;

	_Body.resize(len);
	stream.read(&_Body[0], len);
	if (stream.gcount() < static_cast<std::streamsize>(len))
	{
		// _Body.clear();
		throw std::runtime_error("uncompleate");
	}
}

void Request::handleRequest(std::string &raw)
{
	std::istringstream stream(raw);
	std::string line;

	// printRequest(raw); // print request

	if (!std::getline(stream, line))
		throw std::runtime_error("invalid request");

	if (!line.empty() && line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);

	parseRequestLine(line);
	parseHeaders(stream);
	parseBody(stream);
}

std::string Request::response()
{
	Response rsp = RequestHandler::handle(*this, this->_config);
	return rsp.toString();
}

// void	Request::sendResponse(int clientFd) {

//	Response rsp = Response::build(*this, this->_config);
//	/**/
//	//RequestHandler	rqshd(*this);
//	//std::string Body = rqshd.handle(this->_config);
//	std::string Body = 	rsp.toString();
//	send(clientFd, Body.c_str(), Body.length(), 0);
//}

std::string Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = _Headers.find(key);
	if (it != _Headers.end())
		return it->second;
	return "";
}

size_t Request::getContentLength() const
{
	std::string len = getHeader("Content-Length");
	if (len.empty())
		return 0;
	return std::atoi(len.c_str());
}
