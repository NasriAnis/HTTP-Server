#ifndef SERVER_H
#define SERVER_H

typedef struct { //Typedef holding 
int port; // port,
    int backlog; // backlog (waiting connection),
    int server_fd; //  server file descriptor
} server_t; // Type name that allows the utulisation 
            // of the struct as a typedef

// Pass the structure to the functions that will be used at server.c
int server_init(server_t *srv, int port); 
void server_run(server_t *srv);
void server_close(server_t *srv);

#endif