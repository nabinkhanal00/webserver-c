#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "common.h"

// TODO: make it multithreaded
int main() {
    int listenfd, connfd, n;
    struct sockaddr_in serveaddr;

    uint8_t buff[MAXLINE];
    uint8_t recvline[MAXLINE];

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
    while (1) {
        struct sockaddr_in addr;
        socklen_t addr_len;
        char address[MAXLINE];

        connfd = accept(listenfd, (SA *)&addr, &addr_len);

        inet_ntop(AF_INET, &addr, address, MAXLINE);
        fprintf(stdout, "Connected to: %s\n", address);

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
}
