#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "request.h"

void request_init(http_request_t *req) {
    memset(req, 0, sizeof(http_request_t));
}

int request_parse(http_request_t *req, char *raw_buffer) {
    req->raw_data = raw_buffer;
    char *ptr = raw_buffer;

    // 1. Parse Request Line (Method Path Version)
    req->method = ptr;
    char *space = strchr(ptr, ' ');
    if (!space) return -1;
    *space = '\0';
    ptr = space + 1;

    req->path = ptr;
    space = strchr(ptr, ' ');
    if (!space) return -1;
    *space = '\0';
    ptr = space + 1;

    req->version = ptr;
    char *newline = strstr(ptr, "\r\n");
    if (!newline) return -1;
    *newline = '\0';
    ptr = newline + 2;

    // 2. Parse Headers
    while (ptr && *ptr != '\r' && req->header_count < MAX_HEADERS) {
        req->headers[req->header_count].key = ptr;
        char *colon = strchr(ptr, ':');
        if (!colon) break;
        
        *colon = '\0';
        ptr = colon + 1;
        
        // Skip leading spaces in value
        while (*ptr == ' ') ptr++;
        
        req->headers[req->header_count].value = ptr;
        
        newline = strstr(ptr, "\r\n");
        if (!newline) break;
        
        *newline = '\0';
        ptr = newline + 2;
        req->header_count++;
    }

    // 3. Find Body (after \r\n\r\n)
    char *body_start = strstr(ptr, "\r\n"); // We are already at the end of headers
    if (body_start && *(body_start + 2) == '\0') {
        // No body or we reached the end
        req->body = NULL;
        req->body_len = 0;
    } else {
        req->body = ptr;
        // This is a simple implementation, body length would ideally 
        // come from Content-Length header.
        req->body_len = strlen(ptr);
    }

    return 0;
}

void request_free(http_request_t *req) {
    if (req->raw_data) {
        free(req->raw_data);
    }
}