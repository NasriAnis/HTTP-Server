#include <string.h>
#include "router.h"

void router_init(router_t *rt) {
    rt->count = 0;
}

void router_add(router_t *rt, char *path, route_handler handler) {
    if (rt->count < MAX_ROUTES) {
        rt->entries[rt->count].path = path;
        rt->entries[rt->count].handler = handler;
        rt->count++;
    }
}

void router_match(router_t *rt, const http_request_t *req, http_response_t *res) {
    for (int i = 0; i < rt->count; i++) {
        if (strcmp(rt->entries[i].path, req->path) == 0) {
            rt->entries[i].handler(req, res);
            return;
        }
    }
    
    // Default 404
    res->status_code = 404;
    res->status_message = "Not Found";
    res->body = "404 - Page Not Found";
    res->body_len = strlen(res->body);
}