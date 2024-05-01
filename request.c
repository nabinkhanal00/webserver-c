#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "request.h"

char* trim(char*);

Request* request_create(const char* request_data) {
    char* header;
    char* body;
    char* token;
    Request* req;
    char* data;

    req = malloc(sizeof(Request));
    req->headers = hashmap_create();

    data = malloc(strlen(request_data));
    strcpy(data, request_data);

    char* info_end = strstr(data, "\r\n");

    char* header_end = strstr(data, "\r\n\r\n");
    if (header_end == NULL || info_end == NULL) {
        return NULL;
    }

    header = info_end + 2;
    *header_end = '\0';
    body = header_end + 4;

    req->body = malloc(sizeof(body));
    strcpy((char*)req->body, body);

    // parse http info
    // parse http method
    token = strtok(data, " ");
    if (token == NULL) {
        err_n_die("Method token found to be null");
    }
    req->method = string_to_method(token);

    // parse http url
    token = strtok(NULL, " ");
    if (token == NULL) {
        err_n_die("URL token found to be null");
    }
    char* url = malloc(strlen(token));
    strcpy(url, token);
    req->url = url;

    // parse http version
    token = strtok(NULL, "\r");
    if (token == NULL) {
        err_n_die("Version token found to be null");
    }

    char* version = malloc(strlen(token));
    strcpy(version, token);
    req->version = version;

    char* line = strtok(header, "\r\n");
    while (line != NULL) {
        char* header_end = strstr(line, ":");
        *header_end = 0;
        hashmap_insert(req->headers, trim(line), trim(header_end + 1));
        line = strtok(NULL, "\r\n");
    }

    free(data);

    return req;
}

Method string_to_method(char* input) {
    if (strcmp(input, "GET")) {
        return GET;
    } else if (strcmp(input, "POST")) {
        return POST;
    } else if (strcmp(input, "PUT")) {
        return PUT;
    } else if (strcmp(input, "PATCH")) {
        return PATCH;
    } else if (strcmp(input, "DELETE")) {
        return DELETE;
    } else if (strcmp(input, "OPTION")) {
        return OPTION;
    } else {
        return UNKNOWN;
    }
}

// Function to trim leading and trailing whitespace from a string
char* trim(char* str) {
    // Trim leading whitespace
    while (isspace(*str)) {
        str++;
    }
    // If the entire string is whitespace, return an empty string
    if (*str == '\0') {
        return str;
    }
    // Trim trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }

    // Null-terminate the trimmed string
    *(end + 1) = '\0';

    return str;
}