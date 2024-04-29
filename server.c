
#include "response.h"
#include "server.h"

void* server_worker(void*);
void handle_connection(Server*, int);
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
        if (inet_aton(sc->listen_addr, &addr) == 0) {
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
    uint8_t buff[MAXLINE];
    uint8_t recvline[MAXLINE];
    int n;

    memset(recvline, 0, sizeof(recvline));

    while ((n = read(connfd, recvline, MAXLINE)) > 0) {
        const char* data = bin2hex(recvline, n);
        fprintf(stdout, "\n%s\n\n%s", data, recvline);
        free((void*)data);
        if (recvline[n - 1] == '\n') {
            break;
        }
        memset(recvline, 0, sizeof(recvline));
    }
    if (n < 0) {
        err_n_die("read error");
    }

    snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");
    write(connfd, (char*)buff, strlen((char*)buff));
    close(connfd);
}
