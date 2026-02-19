#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"

int server_init(server_t *srv, int port)
{
    // set the struct defined in serve.h
    srv->port = port;
    srv->backlog = 10;
    srv->server_fd = -1;

    // Fill address info
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(srv->port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Create a socket
    printf("Preparing to create the socket...\n");
    srv->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv->server_fd < 0)
    {
        perror("socket");
        return -1;
    }
    printf("Socket created succesfully !\n");

    // Bind the socket
    printf("Preparing to Bind...\n");
    if (bind(srv->server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return -1;
    }
    printf("Bind succesful !\n");

    // listen
    printf("Preparing to listen...\n");
    if (listen(srv->server_fd, srv->backlog) < 0)
    {
        perror("listen");
        return -1;
    }
    printf("listening...\n");

    return 0;
}

void server_run(server_t *srv)
{
    struct sockaddr_in client_addr;             // Portable type defined by the system
    socklen_t client_len = sizeof(client_addr); // Measures the size of your client_addr struct
                                                // in bytes and stores it in client_len
    while (1)
    {
        // Accepting new connection
        printf("Prepaing to accept new connection...\n");
        int connection_fd = accept(srv->server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (connection_fd < 0)
        {
            perror("accept");
        }

        // Print the Connected client IP
        printf("Connection IP : %s\n", inet_ntoa(client_addr.sin_addr));

        // Close the connection
        close(connection_fd);
    }
}

void server_close(server_t *srv)
{
    close(srv->server_fd);
}