#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>

#define MAX_HEADERS 50

typedef struct {
  char *key;
  char *value;
} http_header_t;

typedef struct {
  char *method;  // GET, POST, etc.
  char *path;    // /index.html
  char *version; // HTTP/1.1
  http_header_t headers[MAX_HEADERS];
  int header_count;
  char *body;
  size_t body_len;
  char *raw_data; // Pointer to the full allocated buffer for easy freeing
} http_request_t;

// Function prototypes
void request_init(http_request_t *req);
int request_parse(http_request_t *req, char *raw_buffer);
void request_free(http_request_t *req);

#endif
