#include "../../includes/Webserv.hpp"

std::string readOutput(int fd)
{
    std::string output;
    char buffer[1024];
    while (true)
    {
        int bread = read(fd, buffer, sizeof(buffer));
        switch (bread)
        {
        case -1:
            throwing("read()");
            break;
        case 0:
            return (output);
        default:
            output += buffer;
        }
    }
}

unsigned int runCGI(std::string fileName)
{
    int pfd[2];
    int fd;

    if (pipe(pfd) < 0)
        throwing("pipe()");
    fd = fork();
    if (fd < 0)
        throwing("fork()");
    if (fd == 0)
    {
        close(pfd[0]);
        char *ptr[2];
        ptr[0] = const_cast<char *>(fileName.c_str());
        ptr[1] = NULL;
        if (dup2(STDOUT_FILENO, pfd[1]) < 0)
            throwing("dup2()");
        if (execve(fileName.c_str(), ptr, NULL) < 0)
            throwing("execve()");
    }
    else
    {
        close(pfd[1]);
        int status;
        wait(&status);
        if (WIFSIGNALED(status))
            throwing("crash in child process");
        return (pfd[0]);
    }
    return 0;
}

void CGIhandler(std::string fileName, std::map<int, t_clients> &client, int fd)
{
    int pfd = runCGI(fileName);
    std::string output = readOutput(pfd);
    client[fd].response = output;
}
