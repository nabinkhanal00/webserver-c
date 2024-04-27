#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"
#include "queue.h"

#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

int get_no_cores() {
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW;
    nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if (count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if (count < 1) {
            count = 1;
        }
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

void handle_connection(int);
void *worker(void *);

pthread_mutex_t queue_lock;
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
        } else {
        }
        inet_ntop(AF_INET, &addr, address, MAXLINE);
        fprintf(stdout, "Connected to: %s\n", address);
        enqueue(q, connfd);
    }
}

void *worker(void *args) {
    int connfd;
    while (1) {
        if (pthread_mutex_lock(&queue_lock) < 0) {
            err_n_die("Locking failed");
        }
        int empty = is_empty(q);
        if (empty) {
            if (pthread_mutex_unlock(&queue_lock) < 0) {
                err_n_die("Unlocking failed");
            }
            continue;
        } else {
            connfd = dequeue(q);
            if (pthread_mutex_unlock(&queue_lock) < 0) {
                err_n_die("Unlocking failed");
            }
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
    sleep(1);
    write(connfd, (char *)buff, strlen((char *)buff));
    close(connfd);
}
