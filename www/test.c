#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buffer[1024];
    size_t n;
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/plain\n\n");
    printf("\r\n");

    while ((n = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
    {
        fwrite(buffer, 1, n, stdout);
    }
    return 0;
}