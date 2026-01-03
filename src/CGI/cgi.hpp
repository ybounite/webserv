#pragma once
# include "../../includes/Webserv.hpp"
// # include "../response/Response.hpp"
// # include "../request/RequestHandler.hpp"

class cgi
{

public:
    int _pipeFd;    
    void CGIhandler(std::string fileName);
    int runCGI(std::string fileName);

    cgi(): _pipeFd(-1) {}
    ~cgi() {}
};

