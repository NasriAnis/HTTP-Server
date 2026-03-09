#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"
#include "request.h"
#include "response.h"

int server_init(server_t *srv, int port, router_t *rt)
{
    srv->port = port;
    srv->backlog = 10;
    srv->server_fd = -1;
    srv->router = rt;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(srv->port);
    addr.sin_addr.s_addr = INADDR_ANY;

    srv->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv->server_fd < 0)
    {
        perror("socket");
        return -1;
    }

    // Set socket options to reuse address (prevents "Address already in use" errors)
    int opt = 1;
    setsockopt(srv->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(srv->server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return -1;
    }

    if (listen(srv->server_fd, srv->backlog) < 0)
    {
        perror("listen");
        return -1;
    }

    return 0;
}

void server_run(server_t *srv)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    printf("Server listening on port %d...\n", srv->port);

    while (1)
    {
        int connection_fd = accept(srv->server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (connection_fd < 0)
        {
            perror("accept");
            continue;
        }

        printf("Connection from : %s\n", inet_ntoa(client_addr.sin_addr));

        // Read request
        char *buffer = malloc(4096);
        int bytes_read = read(connection_fd, buffer, 4095);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';

            http_request_t req;
            request_init(&req);
            if (request_parse(&req, buffer) == 0) {
                http_response_t res;
                response_init(&res);

                // Match route
                router_match(srv->router, &req, &res);

                // Send response
                size_t out_len;
                char *response_str = response_render(&res, &out_len);
                if (response_str) {
                    send(connection_fd, response_str, out_len, 0);
                    free(response_str);
                }
            }
        } else {
            free(buffer);
        }

        close(connection_fd);
    }
}

void server_close(server_t *srv)
{
    if (srv->server_fd >= 0)
        close(srv->server_fd);
}