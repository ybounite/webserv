#pragma once
#include "../../includes/Webserv.hpp"
// # include "../response/Response.hpp"
// # include "../request/RequestHandler.hpp"

class cgi
{

public:
    int _pipeFd;
    void CGIhandler(std::string &fileName, std::string &pathInfo, std::string &QueryString, std::string &body, std::string &method);
    int runCGI(std::string &fileName, std::string &pathInfo, std::string &QueryString, std::string &body, std::string &method);

    cgi() : _pipeFd(-1) {}
    ~cgi() {}
};
