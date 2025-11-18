#pragma once
#include <iostream>
#include <vector>
#include <map>


class ConfigFileReader {
public:
    static std::string read(const std::string &path);
};

class Tokenizer {
public:
    std::vector<std::string> tokenize(const std::string &content);
};

class LocationConfig {
public:
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::vector<std::string> methods;
    std::string upload_path;
    std::string cgi_path;
};


class ServerConfig {
public:
    int listen_port;
    std::string server_name;
    std::string root;
    size_t client_max_body_size;
    std::map<int, std::string> error_pages;
    std::vector<LocationConfig> locations;
};


class Config {
public:
    std::vector<ServerConfig> servers;
};


class Parser {
public:
    Config parse(const std::vector<std::string> &tokens);
private:
    ServerConfig parseServer();
    LocationConfig parseLocation();
};
