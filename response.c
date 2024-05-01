#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "response.h"

Response* response_create(int connfd) {
    Response* rs = malloc(sizeof(Response));
    rs->connfd = connfd;
    return rs;
}
void response_write(Response* rs, char* value) {
    write(rs->connfd, value, strlen(value));
}

void response_destroy(Response* rs) { free(rs); }
