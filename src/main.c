#include <stdio.h>
#include "server.h" 

int main(void)
{
    server_t srv;

    if (server_init(&srv, 8080)){
        perror("failed to init the server \n");
        return -1;
    }
    printf("Server init sucessful...\n");

    server_run(&srv);
    server_close(&srv);

    return 0;
}