#include "server.h"

void handle_hello(Request* re, Response* rs) {
    response_write(
        rs,
        "HTTP/1.1 200 OK\r\n"
        "\r\n"
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Web Server in C</title>"
        "</head>"
        "<body>"
        "Hello World"
        "</body>"
        "</html>\r\n");
}
int main() {
    ServerConfig sc;
    sc.listen_addr = "127.0.0.1";
    sc.listen_port = 3000;

    Server* s = server_create(&sc);

    server_handle(s, GET, "/", handle_hello);
    server_listen(s);
}
