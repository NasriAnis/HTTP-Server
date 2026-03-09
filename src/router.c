#include <stdio.h>
#include <stdlib.h>
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
    
    // Attempt to load 404.html
    FILE *f = fopen("public/404.html", "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *content = malloc(fsize + 1);
        fread(content, 1, fsize, f);
        fclose(f);
        content[fsize] = 0;
        res->body = content;
        res->body_len = fsize;
        res->content_type = "text/html";
    } else {
        res->body = "<h1>404 Not Found</h1>";
        res->body_len = strlen(res->body);
    }
}