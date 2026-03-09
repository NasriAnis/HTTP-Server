#ifndef RESPONSE_H
#define RESPONSE_H

#include <stddef.h>

typedef struct {
    int status_code;        // e.g. 200, 404
    char *status_message;   // e.g. "OK", "Not Found"
    char *content_type;     // e.g. "text/html"
    char *body;
    size_t body_len;
} http_response_t;

// Prototypes
void response_init(http_response_t *res);
char *response_render(http_response_t *res, size_t *out_len);
void response_free(http_response_t *res);

#endif