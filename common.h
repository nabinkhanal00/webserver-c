#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXLINE 4096
#define MAX_CONNECTIONS 10
#define SERVER_PORT 4444
#define SA struct sockaddr

void err_n_die(const char* , ...);
char *bin2hex(const unsigned char* , size_t );
