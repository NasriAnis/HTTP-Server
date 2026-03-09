#ifndef SERVER_H
#define SERVER_H

#include "router.h"

typedef struct {
    int port;
    int backlog;
    int server_fd;
    router_t *router;
} server_t;

int server_init(server_t *srv, int port, router_t *rt); 
void server_run(server_t *srv);
void server_close(server_t *srv);

#endif