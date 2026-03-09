#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "response.h"

void response_init(http_response_t *res) {
    memset(res, 0, sizeof(http_response_t));
    res->status_code = 200;
    res->status_message = "OK";
    res->content_type = "text/plain";
}

char *response_render(http_response_t *res, size_t *out_len) {
    // Basic rendering: Status-Line\r\nContent-Type\r\nContent-Length\r\n\r\nBody
    char *buffer = malloc(1024 + res->body_len); // Basic pre-allocation
    if (!buffer) return NULL;

    sprintf(buffer, 
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        res->status_code, res->status_message, res->content_type, res->body_len);
    
    size_t header_len = strlen(buffer);
    if (res->body && res->body_len > 0) {
        memcpy(buffer + header_len, res->body, res->body_len);
    }

    *out_len = header_len + res->body_len;
    return buffer;
}

void response_free(http_response_t *res) {
    // Only free if the body was dynamic
    // In this basic version, we assume strings are literals for now.
}