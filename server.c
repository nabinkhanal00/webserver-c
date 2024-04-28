#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"
#include "queue.h"

void handle_connection(int);
void *worker(void *);

pthread_mutex_t queue_lock;
pthread_cond_t queue_cond;
Queue *q;

int main() {
    int listenfd, connfd, n;
    struct sockaddr_in serveaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("error while creating a socket");
    }

    bzero(&serveaddr, sizeof(serveaddr));

    serveaddr.sin_family = AF_INET;
    // host to network byte long
    serveaddr.sin_addr.s_addr = htons(INADDR_ANY);
    serveaddr.sin_port = htons(SERVER_PORT);

    if (bind(listenfd, (SA *)&serveaddr, sizeof(serveaddr)) < 0) {
        err_n_die("bind error");
    }

    if (listen(listenfd, MAX_CONNECTIONS) < 0) {
        err_n_die("listen error");
    } else {
        fprintf(stdout, "Listening on port %d...\n", SERVER_PORT);
        fflush(stdout);
    }

    // utilize half of the present cores
    int no_of_cores = get_no_cores();
    q = create_queue();

    pthread_t *threads = (pthread_t *)malloc(no_of_cores * sizeof(pthread_t));

    if (pthread_mutex_init(&queue_lock, NULL) < 0) {
        err_n_die("Unable to create mutex lock.");
    }
    if (pthread_cond_init(&queue_cond, NULL) < 0) {
        err_n_die("Unable to create mutex lock.");
    }
    for (int i = 0; i < no_of_cores; i++) {
        if (pthread_create(&threads[i], NULL, worker, NULL) < 0) {
            err_n_die("Unable to create threads.");
        }
    }

    while (1) {
        struct sockaddr_in addr;
        socklen_t addr_len;
        char address[MAXLINE];

        connfd = accept(listenfd, (SA *)&addr, &addr_len);
        if (connfd == -1) {
            err_n_die("Invalid Connection ID.");
            continue;
        }
        inet_ntop(AF_INET, &addr, address, MAXLINE);
        fprintf(stdout, "Connected to: %s\n", address);
        if (pthread_mutex_lock(&queue_lock) < 0) {
            err_n_die("Locking failed");
        }
        enqueue(q, connfd);
        if (pthread_mutex_unlock(&queue_lock) < 0) {
            err_n_die("Unlocking failed");
        }
        pthread_cond_signal(&queue_cond);
    }
    pthread_mutex_destroy(&queue_lock);
    pthread_cond_destroy(&queue_cond);
}

void *worker(void *args) {
    int connfd;
    while (1) {
        if (pthread_mutex_lock(&queue_lock) < 0) {
            err_n_die("Locking failed");
        }
        while (is_empty(q)) {
            if (pthread_cond_wait(&queue_cond, &queue_lock) < 0) {
                err_n_die("Unlocking failed");
            }
        }
        connfd = dequeue(q);
        if (pthread_mutex_unlock(&queue_lock) < 0) {
            err_n_die("Unlocking failed");
        }
        handle_connection(connfd);
    }
    return NULL;
}

void handle_connection(int connfd) {
    uint8_t buff[MAXLINE];
    uint8_t recvline[MAXLINE];
    int n;

    memset(recvline, 0, sizeof(recvline));

    while ((n = read(connfd, recvline, MAXLINE)) > 0) {
        const char *data = bin2hex(recvline, n);
        fprintf(stdout, "\n%s\n\n%s", data, recvline);
        free((void *)data);
        if (recvline[n - 1] == '\n') {
            break;
        }
        memset(recvline, 0, sizeof(recvline));
    }
    if (n < 0) {
        err_n_die("read error");
    }
    snprintf((char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");
    write(connfd, (char *)buff, strlen((char *)buff));
    close(connfd);
}
