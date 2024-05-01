#include <netinet/tcp.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "hashmap.h"
#include "request.h"
#include "response.h"
#include "server.h"

void* server_worker(void*);
void handle_connection(Server*, int);
void handle_request(Server*, Request*, Response*);

unsigned int find_empty_port(unsigned int start_port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return 0;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Bind to localhost

    // Iterate through port numbers starting from start_port
    for (int port = start_port; port <= 65535; port++) {
        addr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            // Binding failed, indicating the port is available
            continue;
        } else {
            close(sockfd);
            return port;
        }
    }

    // If no available port is found
    close(sockfd);
    return 0;
}
Server* server_create(ServerConfig* sc) {
    Server* s = malloc(sizeof(Server));

    struct sockaddr_in serveaddr;
    if ((s->listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("error while creating a socket");
    }

    int flag = 1;
    int result =
        setsockopt(s->listenfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    if (result < 0) {
        err_n_die("couldnot disable nagle algorithm");
    }
    serveaddr.sin_family = AF_INET;
    // host to network byte long
    if (sc->listen_port) {
        if (sc->listen_port == find_empty_port(sc->listen_port)) {
            serveaddr.sin_port = htons(sc->listen_port);
        } else {
            err_n_die("the port %d is not available", sc->listen_port);
        }
    } else {
        unsigned int port = find_empty_port(DEFAULT_LISTEN_PORT);
        if (port == 0) {
            err_n_die("cannot find an available port");
        } else {
            serveaddr.sin_port = htons(port);
        }
    }

    struct in_addr addr;
    if (sc->listen_addr != NULL) {
        if (inet_pton(AF_INET, sc->listen_addr, &addr) == 0) {
            err_n_die("invalid ip address: %s\n", sc->listen_addr);
        }
    } else {
        addr.s_addr = htonl(INADDR_LOOPBACK);
    }
    serveaddr.sin_addr.s_addr = addr.s_addr;

    if (bind(s->listenfd, (SA*)&serveaddr, sizeof(serveaddr)) < 0) {
        err_n_die("cannot bind fd to the socket");
    }

    s->connection_queue = queue_create();
    s->handler_map = hashmap_create(-1);
    s->serveaddr = malloc(sizeof(serveaddr));
    memcpy(s->serveaddr, &serveaddr, sizeof(serveaddr));

    return s;
}

int server_listen(Server* s) {
    if (listen(s->listenfd, MAX_CONNECTIONS) < 0) {
        err_n_die("listen error");
    } else {
        printf("Listening on port %d...\n", ntohs(s->serveaddr->sin_port));
    }
    int no_of_cores = get_no_cores();
    s->handler_threads = (pthread_t*)malloc(no_of_cores * sizeof(pthread_t));

    if (pthread_mutex_init(&s->queue_lock, NULL) < 0) {
        err_n_die("Unable to create mutex lock.");
    }
    if (pthread_cond_init(&s->queue_cond, NULL) < 0) {
        err_n_die("Unable to create mutex lock.");
    }

    for (int i = 0; i < no_of_cores; i++) {
        if (pthread_create(
                &s->handler_threads[i], NULL, server_worker, (void*)s) < 0) {
            err_n_die("Unable to create threads.");
        }
    }

    while (1) {
        int connfd;
        struct sockaddr_in addr;
        socklen_t addr_len;
        char address[MAXLINE];

        connfd = accept(s->listenfd, (SA*)&addr, &addr_len);
        if (connfd == -1) {
            err_n_die("got invalid connid");
        }

        int flag = 1;
        int result =
            setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
        if (result < 0) {
            err_n_die("couldnot disable nagle algorithm");
        }
        inet_ntop(AF_INET, &addr, address, MAXLINE);
        fprintf(stdout, "Connected to: %s\n", address);
        if (pthread_mutex_lock(&s->queue_lock) < 0) {
            err_n_die("Locking failed");
        }
        queue_enqueue(s->connection_queue, connfd);
        if (pthread_mutex_unlock(&s->queue_lock) < 0) {
            err_n_die("Unlocking failed");
        }
        pthread_cond_signal(&s->queue_cond);
    }
    pthread_mutex_destroy(&s->queue_lock);
    pthread_cond_destroy(&s->queue_cond);
}

void* server_worker(void* args) {
    Server* s = args;
    int connfd;
    while (1) {
        if (pthread_mutex_lock(&s->queue_lock) < 0) {
            err_n_die("Locking failed");
        }
        while (queue_is_empty(s->connection_queue)) {
            if (pthread_cond_wait(&s->queue_cond, &s->queue_lock) < 0) {
                err_n_die("Unlocking failed");
            }
        }
        connfd = queue_dequeue(s->connection_queue);
        if (pthread_mutex_unlock(&s->queue_lock) < 0) {
            err_n_die("Unlocking failed");
        }
        handle_connection(s, connfd);
    }
    return NULL;
}

void handle_connection(Server* s, int connfd) {
    uint8_t recvline[MAXLINE];
    int n;

    memset(recvline, 0, sizeof(recvline));

    char* request_data = malloc(MAXLINE);
    unsigned int received = 0;
    unsigned int size = MAXLINE;
    memset(request_data, 0, MAXLINE);

    while ((n = read(connfd, recvline, MAXLINE)) > 0) {
        if (size - received - n <= 0) {
            request_data = realloc(request_data, 2 * size);
            size *= 2;
        }
        memcpy(request_data + received, recvline, n);
        received += n;
        if (recvline[n - 1] == '\n') {
            request_data[received] = '\0';
            Request* re = request_create(request_data);
            Response* rs = response_create(connfd);
            handle_request(s, re, rs);
            response_destroy(rs);
            request_destroy(re);
            free(request_data);
            request_data = malloc(MAXLINE);
            memset(request_data, 0, MAXLINE);
            received = 0;
            size = MAXLINE;
            break;
        }
        memset(recvline, 0, sizeof(recvline));
    }
    free(request_data);
    if (n < 0) {
        err_n_die("read error");
    }

    close(connfd);
}

void handle_request(Server* s, Request* re, Response* rs) {
    char* method = method_to_string(re->method);
    unsigned int length = strlen(method) + 1 + strlen(re->url) + 1;
    char* mkey = malloc(length);
    strcpy(mkey, method);
    strcpy(mkey + strlen(method), " ");
    strcpy(mkey + strlen(method) + 1, re->url);
    mkey[length - 1] = '\0';
    Handler handler = hashmap_get(s->handler_map, mkey);
    free(mkey);
    if (handler == NULL) {
        printf("handler not found\n");
        return;
    }
    handler(re, rs);
}
void server_handle(Server* s, Method m, Path p, Handler h) {
    char* method = method_to_string(m);
    unsigned int length = strlen(method) + 1 + strlen(p) + 1;
    char* mkey = malloc(length);
    strcpy(mkey, method);
    strcpy(mkey + strlen(method), " ");
    strcpy(mkey + strlen(method) + 1, p);
    mkey[length - 1] = '\0';
    hashmap_insert(s->handler_map, mkey, h);
}
