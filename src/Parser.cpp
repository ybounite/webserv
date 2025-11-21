#include "../includes/Webserv.hpp"

Config Parser::parse(const std::vector<std::string> &tokens)
{
    Config config;
    unsigned long i;
    while (i < tokens.size())
    {
        if (tokens[i] == "server")
            config.servers.push_back(parseServer(tokens));
        else
            throw std::runtime_error("Unexpected token: " + tokens[i]);
    }
    
}
int parseListen(const std::vector<std::string> &tokens, unsigned long i)
{

}
std::string parseStringDirective(const std::vector<std::string> &tokens, unsigned long i)
{

}

size_t parseSizeDirective2(const std::vector<std::string> &tokens, unsigned long i)
{

}


 ServerConfig Parser::parseServer(const std::vector<std::string> &tokens)
 {
    ServerConfig server;
    unsigned long i = 2;
    if (tokens[0] != "server")
        throw std::runtime_error("Unexpected token: " + tokens[0]);
    if (tokens[0] != "{")
        throw std::runtime_error("Unexpected token: " + tokens[1]);
    while (tokens[i] != "}")
    {
        if (tokens[i] == "listen")
            server.listen_port = parseListen(tokens, i);
        else if (tokens[i] == "root")
            server.root = parseStringDirective(tokens, i);
        else if (tokens[i] == "server_name")
            server.server_name = parseStringDirective(tokens, i);
        else if (tokens[i] == "client_max_body_size")
            server.client_max_body_size = parseSizeDirective2(tokens, i);
        // else if (tokens[i] == "error_page")
        //     parseErrorPage(server);
        else if (tokens[i] == "location")
            server.locations.push_back(parseLocation());
        else
            throw std::runtime_error("Unknown directive: " + tokens[i]);
        i++;
    }
    if (tokens[i] != "}")
        throw std::runtime_error("Unexpected token: " + tokens[1]);
    return server;
 }

 LocationConfig Parser::parseLocation()
 {

 }
