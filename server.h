#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "hashmap.h"
#include "queue.h"
#include "request.h"
#include "response.h"

#define MAXLINE 4096
#define MAX_CONNECTIONS 1024
#define SA struct sockaddr
#define DEFAULT_LISTEN_PORT 3000
typedef struct Server {
    int listenfd;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_cond;
    Queue *connection_queue;
    int no_of_threads;
    struct sockaddr_in *serveaddr;
    Hashmap *handler_map;
    pthread_t *handler_threads;
} Server;

typedef struct ServerConfig {
    const char *listen_addr;
    unsigned int listen_port;
} ServerConfig;

typedef char *Path;
typedef void (*Handler)(Request *, Response *);
Server *server_create(ServerConfig *);
int server_listen(Server *);
void server_handle(Server *, Method, Path, Handler);
#endif