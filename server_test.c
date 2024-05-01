#include "server.h"
int main() {
    ServerConfig sc;
    sc.listen_addr = "127.0.0.1";
    sc.listen_port = 3000;

    Server* s = server_create(&sc);

    server_listen(s);
}
