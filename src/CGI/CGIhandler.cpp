#include "../../includes/Webserv.hpp"

static char *dupEnv(const std::string &s)
{
    char *env = new char[s.size() + 1];
    std::strcpy(env, s.c_str());
    return env;
}

char **buildCgiEnv(stCgiInfo &info)
{
    std::vector<std::string> env;

    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=" + info.HttpV);
    env.push_back("REQUEST_METHOD=" + info.Method);
    env.push_back("SCRIPT_FILENAME=" + info.FileName);
    env.push_back("SCRIPT_NAME=" + info.FileName);
    env.push_back("QUERY_STRING=" + info.QueryString);
    env.push_back("CONTENT_LENGTH=" + to_string(info.ContentLenght));
    env.push_back("CONTENT_TYPE=" + info.ContentType);
    env.push_back("SERVER_NAME=" + info.ServerName);
    env.push_back("PATH_INFO=" + info.PathInfo);
    env.push_back("SERVER_PORT=" + to_string(info.Port));
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

int cgi::runCGI(stCgiInfo &info)
{
    int pfd1[2];
    if (info.Method == "POST")
    {

        if (pipe(pfd1) < 0)
            return -1;
        write(pfd1[1], info.Body.c_str(), info.ContentLenght);
        close(pfd1[1]);
    }
    int pfd[2];

    if (pipe(pfd) < 0)
        return -1;

    _Pid = fork();
    if (_Pid < 0)
        return -1;

    if (_Pid == 0)
    {
        close(pfd[0]);

        dup2(pfd[1], STDOUT_FILENO);
        dup2(pfd1[0], STDIN_FILENO);
        close(pfd[1]);

        char *argv_exec[2];
        argv_exec[0] = const_cast<char *>(info.FileName.c_str());
        argv_exec[1] = NULL;
        char **envp = buildCgiEnv(info);
        if (execve(info.FileName.c_str(), argv_exec, envp))
        {
            return -1;
        }
    }

    close(pfd[1]);
    int status;
    waitpid(_Pid, &status, WNOHANG);
    return pfd[0];
}

void cgi::CGIhandler(stCgiInfo &info)
{
    if (access(info.FileName.c_str(), X_OK) == -1)
    {
        Msg::error("ENTERED");
        _pipeFd = -1;
        return;
    }
    _pipeFd = runCGI(info);
}
