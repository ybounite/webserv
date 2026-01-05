#include "../../includes/Webserv.hpp"

static char *dupEnv(const std::string &s)
{
    char *env = new char[s.size() + 1];
    std::strcpy(env, s.c_str());
    return env;
}

char **buildCgiEnv(
    const std::string &method,
    const std::string &filePath,
    const std::string &query,
    const std::string &bodySize,
    const std::string &contentType)
{
    std::vector<std::string> env;

    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("REQUEST_METHOD=" + method);
    env.push_back("SCRIPT_FILENAME=" + filePath);
    env.push_back("SCRIPT_NAME=" + filePath);
    env.push_back("QUERY_STRING=" + query);
    env.push_back("CONTENT_LENGTH=" + bodySize);
    env.push_back("CONTENT_TYPE=" + contentType);
    env.push_back("SERVER_NAME=localhost");
    env.push_back("SERVER_PORT=8080");
    env.push_back("REDIRECT_STATUS=200"); // PHP needs this

    char **envp = new char *[env.size() + 1];

    for (size_t i = 0; i < env.size(); i++)
        envp[i] = dupEnv(env[i]);

    envp[env.size()] = NULL;
    return envp;
}
void throwing(std::string fct)
{
    throw std::runtime_error(fct + std::string("failed: ") + strerror(errno));
}

int cgi::runCGI(std::string &fileName, std::string &pathInfo, std::string &QueryString, std::string &body, std::string &method)
{
    int pfd[2];

    if (pipe(pfd) < 0)
        throwing("pipe()");

    int fd = fork();
    if (fd < 0)
        throwing("fork()");

    if (fd == 0)
    {
        close(pfd[0]);

        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[1]);

        char *argv_exec[2];
        argv_exec[0] = const_cast<char *>(fileName.c_str());
        argv_exec[1] = NULL;
        char **envp = buildCgiEnv("GET", fileName, "", "0", "");
        if (execve(fileName.c_str(), argv_exec, envp))
        {
            return -1;
        }
    }

    close(pfd[1]);
    int status;
    waitpid(fd, &status, WNOHANG);
    return pfd[0];
}

void cgi::CGIhandler(std::string &fileName, std::string &pathInfo, std::string &QueryString, std::string &body, std::string &method)
{

    if (access(fileName.c_str(), X_OK) == -1)
    {
        Msg::error("ENTERED");
        _pipeFd = -1;
        return;
    }
    _pipeFd = runCGI(fileName, pathInfo, QueryString, body, method);
}
