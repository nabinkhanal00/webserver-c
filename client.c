#include "common.h"

int main(int argc, const char* argv[]) {
    int sockfd, n;
    int sendbytes;

    int server_port;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    if (argc < 3) {
        err_n_die("usage: %s <server address> <port>", argv[0]);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("error while creating a socket.");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    server_port = atoi(argv[2]);
    servaddr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        err_n_die("inet_pton error for %s ", argv[1]);
    }

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
        err_n_die("connect failed");
    }

    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    if (write(sockfd, sendline, sendbytes) != sendbytes) {
        err_n_die("write error");
    }
    memset(recvline, 0, MAXLINE);

    while (1) {
        n = read(sockfd, recvline, MAXLINE);
        if (n < 0) {
            err_n_die("read_error");
        } else if (n == 0) {
            exit(0);
        } else {
            printf("%s", recvline);
            memset(recvline, 0, MAXLINE);
        }
    }
}
