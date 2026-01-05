#pragma once
#include "../../includes/Webserv.hpp"
// # include "../response/Response.hpp"
// # include "../request/RequestHandler.hpp"

class cgi
{

public:
    int _pipeFd;
    void CGIhandler(stCgiInfo &info);
    int runCGI(stCgiInfo &info);

    cgi() : _pipeFd(-1) {}
    ~cgi() {}
};
