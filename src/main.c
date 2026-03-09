#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "router.h"

// Helper function to read a file and put it in the response body
void serve_static_file(const char *filename, http_response_t *res) {
    char path[256];
    snprintf(path, sizeof(path), "public/%s", filename);

    FILE *f = fopen(path, "rb");
    if (!f) {
        res->status_code = 404;
        res->status_message = "Not Found";
        res->body = "File not found";
        res->body_len = strlen(res->body);
        return;
    }

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
}

void home_handler(const http_request_t *req, http_response_t *res) {
    (void)req;
    serve_static_file("index.html", res);
}

void about_handler(const http_request_t *req, http_response_t *res) {
    (void)req;
    serve_static_file("about.html", res);
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