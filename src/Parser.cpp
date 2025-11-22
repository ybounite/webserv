#include "../includes/Webserv.hpp"

Config Parser::parse(const std::vector<std::string> &tokens)
{
    Config config;
    unsigned long i = 0;
    while (i < tokens.size())
    {
        if (tokens[i] == "server")
            config.servers.push_back(parseServer(tokens, i));
        else
            throw std::runtime_error("Unexpected token: in `parse` " + tokens[i]);
        i++;
    }
    return config;
}

int parseListen(const std::vector<std::string> &tokens, unsigned long &i)
{
    i++;
    if (tokens[i].find_first_not_of("0123456789 ") != std::string::npos)
        throw std::runtime_error("Unexpected token: should be 0123456789 but token is " + tokens[i]);
    char *end ;
    double result = std::strtod(tokens[i].c_str(), &end);
    int number = static_cast<int>(result);
    if (*end == '\0')
    {
        if (number > 65535 || number < 1)
            throw std::runtime_error("Unexpected token: should be number > 65535 || number < 1 but token is " + tokens[i]);
        i++;
        return number;
    }
    throw std::runtime_error("Unexpected token: " + tokens[i]);
}

size_t parseSizeDirective2(const std::vector<std::string> &tokens, unsigned long &i)
{
    i++;

    const std::string &tok = tokens[i];

    if (tok.size() < 2)
        throw std::runtime_error("Invalid size format: " + tok);

    if (tok[tok.size() - 1] != 'M')
        throw std::runtime_error("Size must end with 'M': " + tok);

    std::string number = tok.substr(0, tok.size() - 1);

    if (number.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("Invalid number in size: " + tok);

    size_t size = std::atoi(number.c_str()) * 1024 * 1024;
    i++;
    return size;
}

void parseErrorPage(ServerConfig &server, const std::vector<std::string> &tokens, unsigned long &i)
{
    i++;
    std::string path;
    unsigned long error;
    if (tokens[i].find_first_not_of("0123456789 ") != std::string::npos)
        throw std::runtime_error("Unexpected token: should be 0123456789 but token is " + tokens[i]);
    char *end ;
    double result = std::strtod(tokens[i].c_str(), &end);
    error = static_cast<int>(result);
    if (*end == '\0')
    {
        if (error > 599 || error < 400)
            throw std::runtime_error("Unexpected token: should be error > 599 || error < 400 but token is " + tokens[i]);
        i++;
    }
    path = tokens[i];
    i++;
    server.error_pages[error] = path;
}

void expect(const std::vector<std::string> &tokens, unsigned long &i)
{
    if (tokens[i] == ";")
        i++;
    else
        throw std::runtime_error("Should be : |;| but token is : " + tokens[i]);
}
ServerConfig Parser::parseServer(const std::vector<std::string> &tokens, unsigned long &i)
{
    ServerConfig server;
    if (tokens[i] != "server")
        throw std::runtime_error("Unexpected token: should be `server` but token is " + tokens[i]);
    if (tokens[i + 1] != "{")
        throw std::runtime_error("Unexpected token: should be `{` but token is" + tokens[i + 1]);
    i += 2;
    while (tokens[i] != "}")
    {
        if (tokens[i] == "listen")
        {
            server.listen_port = parseListen(tokens, i);
            expect(tokens, i);
        }
        else if (tokens[i] == "root")
        {
            server.root = tokens[++i];
            // std::cout << server.root << std::endl;
            i++;
            // std::cout << tokens[i] << std::endl;
            // std::cout << "1" << std::endl;
            expect(tokens, i);
            // exit(1);
        }
        else if (tokens[i] == "index")
        {
            server.index = tokens[++i];
            i++;
            // std::cout << "2" << std::endl;
            expect(tokens, i);
        }
        else if (tokens[i] == "server_name")
        {
            server.server_name = tokens[++i];
            i++;
            // std::cout << "3" << std::endl;
            expect(tokens, i);
        }
        else if (tokens[i] == "client_max_body_size")
        {
            server.client_max_body_size = parseSizeDirective2(tokens, i);
            // std::cout << "4" << std::endl;
            expect(tokens, i);
        }
        else if (tokens[i] == "error_page")
        {
            parseErrorPage(server, tokens, i);
            // std::cout << "5" << std::endl;
            expect(tokens, i);
        }
        else if (tokens[i] == "location")
        {
            server.locations.push_back(parseLocation(tokens, i));
            // std::cout << "6" << std::endl;
            // expect(tokens, i);
        }
        else
            throw std::runtime_error("Unexpected token in parseServer: " + tokens[i]);
    }
    if (tokens[i] != "}")
        throw std::runtime_error("Unexpected token in parseServer: should be `}` but token is " + tokens[i]);
    return server;
 }

 LocationConfig Parser::parseLocation(const std::vector<std::string> &tokens, unsigned long &i)
 {
    LocationConfig location;
    if (tokens[i] != "location")
        throw std::runtime_error("Unexpected token in parseLocation: should be `location` but token is  " + tokens[i]);
    if (tokens[i + 2] != "{")
        throw std::runtime_error("Unexpected token in parseLocation: should be { but token is  " + tokens[i + 2]);
    location.path = tokens[i + 1];
    i += 3;
    while (tokens[i] != "}")
    {
        if (tokens[i] == "autoindex")
        {
            i++;
            if (tokens[i] == "off")
                location.autoindex = false;
            else if (tokens[i] == "on")
                location.autoindex = true; 
            else
                throw std::runtime_error("Unexpected token in parseLocation: should be `off or on ` but " + tokens[i]);
            i++;
            expect(tokens, i);
        }
        else if (tokens[i] == "upload_path")
        {
            i++;
            location.upload_path = tokens[i];
            i++;
            expect(tokens, i);
        }
        else if (tokens[i] == "cgi_path")
        {
            i++;
            location.cgi_path = tokens[i];
            i++;
            expect(tokens, i);
        }
        else if (tokens[i] == "root")
        {
            location.root = tokens[++i];
            i++;
            expect(tokens, i);
        }
        else if (tokens[i] == "index")
        {
            location.index = tokens[++i];
            i++;
            expect(tokens, i);
        }
        else if (tokens[i] == "methods")
        {
            i++;
            while (tokens.size() > i && tokens[i] != ";")
            {
                location.methods.push_back(tokens[i]);
                i++;
            }
            expect(tokens, i);
        }
        else
            throw std::runtime_error("Unexpected token in parseLocation: " + tokens[i]);
    }
    if (tokens[i] != "}")
        throw std::runtime_error("Unexpected token in parseLocation: should be `}` but token is " + tokens[i]);
    i++;
    return location;
 }
