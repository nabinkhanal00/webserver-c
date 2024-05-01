#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct Response {
    int connfd;
} Response;

Response* response_create(int);
void response_write_header();
void response_write(Response*, char*);
void response_destroy(Response*);
#endif