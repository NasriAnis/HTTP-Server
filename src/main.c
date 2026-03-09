#include <stdio.h>
#include <string.h>
#include "server.h"
#include "router.h"

void home_handler(const http_request_t *req, http_response_t *res) {
    (void)req; // Unused
    res->status_code = 200;
    res->status_message = "OK";
    res->content_type = "text/html";
    res->body = "<h1>Welcome to the Home Page!</h1><p>The server is working.</p>";
    res->body_len = strlen(res->body);
}

void about_handler(const http_request_t *req, http_response_t *res) {
    (void)req;
    res->status_code = 200;
    res->status_message = "OK";
    res->content_type = "text/html";
    res->body = "<h1>About Us</h1><p>This is a custom C HTTP server.</p>";
    res->body_len = strlen(res->body);
}

int main(void)
{
    router_t rt;
    router_init(&rt);
    router_add(&rt, "/", home_handler);
    router_add(&rt, "/home", home_handler);
    router_add(&rt, "/about", about_handler);

    server_t srv;
    if (server_init(&srv, 8080, &rt) != 0){
        fprintf(stderr, "Failed to initialize the server.\n");
        return -1;
    }

    server_run(&srv);
    server_close(&srv);

    return 0;
}