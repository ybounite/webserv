#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buffer[1024];
    size_t n;
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/plain\n\n");
    printf("\r\n");

    while (1)
    {
        printf("hehe\n");
    }
    return 0;
}