#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "queue.h"

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
    pthread_t *handler_threads;
} Server;

typedef struct ServerConfig {
    const char *listen_addr;
    unsigned int listen_port;
} ServerConfig;


Server *server_create(ServerConfig *);
int server_listen(Server *);
#endif