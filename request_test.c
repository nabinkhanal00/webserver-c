#include "request.h"

int main() {
    Request* req = request_create(
        "GET https://nabinkhanal00.com.np "
        "HTTP/1.1\r\nname:nabin\r\nsurname:khanal\r\n\r\nThis is the data");
}
