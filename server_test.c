#include "request.h"
#include "response.h"
#include "server.h"

void handle_hello(Request* re, Response* rs) {
    printf("I am inside the handler");
    fflush(stdout);
    response_write(rs, "hello world");
}
int main() {
    ServerConfig sc;
    sc.listen_addr = "127.0.0.1";
    sc.listen_port = 3000;

    Server* s = server_create(&sc);

    printf("the address of the handler is %p\n", handle_hello);
    server_handle(s, GET, "/", handle_hello);
    server_listen(s);
}
