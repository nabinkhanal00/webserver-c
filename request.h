#ifndef REQUEST_H
#define REQUEST_H

#include "hashmap.h"

typedef enum Method { GET, POST, PUT, PATCH, DELETE, OPTION, UNKNOWN } Method;

Method string_to_method(char*);

typedef struct Request {
    enum Method method;
    const char* url;
    const char* version;
    struct Hashmap* headers;
    const char* body;

} Request;

Request* request_create(const char*);
#endif