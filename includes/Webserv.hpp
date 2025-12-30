#pragma once
# include <iostream>
// # include <fstream>
# include <fcntl.h>
# include <cstring>
# include <string> 
# include <unistd.h>
# include <dirent.h>
# include <sstream>
// # include <bits/stdc++.h>
# include <cstdlib>
# include <fstream>
# include <sstream>
# include <cctype>
# include <ctime>
# include <cstdarg>

/* STL Containers */
# include <map>
# include <set>
# include <vector>
# include <algorithm>
# include <iterator>
# include <list>

/* System */
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <unistd.h>
// # include <machine/types.h>
# include <signal.h>

/* Network */
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

// epoll
#include <sys/epoll.h>

// colors
#define RESET          "\x1B[0m"
#define RED            "\x1B[31m"
#define LIGHT_RED      "\x1B[91m"
#define WHITE          "\x1B[37m"
#define BLINK           "\x1b[5m"`
#define YELLOW         "\x1B[33m"
#define LIGHT_BLUE     "\x1B[94m"
#define CYAN           "\x1B[36m"
#define DARK_GREY      "\x1B[90m"
#define LIGHTMAGENTA   "\x1B[95m"
#define GREEN          "\x1B[32m"
#define LIGHT_GREEN    "\x1B[92m"


// classes

class Msg {
public:
    // Generic printer with any color
    static void print(const std::string& mode, const std::string& msg, const std::string& color);

    // Fixed log types
    static void info(const std::string& msg);
    static void success(const std::string& msg);
    static void warning(const std::string& msg);
    static void error(const std::string& msg);
    static void debug(const std::string& msg);
};



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
    // (bonus)
    int return_code;
    std::string return_url;
};


class ServerConfig {
public:
    std::map<std::string, std::map<std::string, std::string> > sessions;
    int listen_port;
    std::string index;
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
    ServerConfig parseServer(const std::vector<std::string> &tokens, unsigned long &i);
    LocationConfig parseLocation(const std::vector<std::string> &tokens, unsigned long &i);
};


// helper function

void parse_config(Config &config);

// global class
//extern class   Config  GbConfig;

template <typename T>
std::string to_string(T &value) {
    std::ostringstream  oss;
    oss << value;
    return oss.str();
}

# include "../src/server/Server.hpp"
# include "../src/request/Request.hpp"
# include "../src/response/Response.hpp"
# include "../src/request/RequestHandler.hpp"



//////////////////////


std::string getCookieValue(const Request &req, std::string key);
void createNewSession(Request &req, ServerConfig &config);
std::string generateSessionId();