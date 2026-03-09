#ifndef ROUTER_H
#define ROUTER_H

#include "request.h"
#include "response.h"

typedef void (*route_handler)(const http_request_t*, http_response_t*);

typedef struct {
    char *path;
    route_handler handler;
} route_entry_t;

#define MAX_ROUTES 100

typedef struct {
    route_entry_t entries[MAX_ROUTES];
    int count;
} router_t;

void router_init(router_t *rt);
void router_add(router_t *rt, char *path, route_handler handler);
void router_match(router_t *rt, const http_request_t *req, http_response_t *res);

#endif