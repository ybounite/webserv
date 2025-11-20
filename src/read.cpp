#include "../includes/Webserv.hpp"

std::string ConfigFileReader::read(const std::string &path)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        throw std::runtime_error("cannot open this file " + path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
