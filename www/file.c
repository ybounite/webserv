#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main(void)
{
    char **env = environ;

    printf("Status: 200 OK\r\n");
    printf("\r\n");  // END OF HEADERS (VERY IMPORTANT)

    printf("<!DOCTYPE html>\n");
    printf("<html><head><title>CGI Env</title></head><body>\n");
    printf("<h1>CGI Environment</h1>\n");
    printf("<ul>\n");

    while (*env)
    {
        printf("<li>%s</li>\n", *env);
        env++;
    }

    printf("</ul>\n");
    printf("</body></html>\n");

    return 0;
}
