#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "hashmap.h"
#include "request.h"

char* trim(char*);

Request* request_create(const char* request_data) {
    char* header;
    char* body;
    char* token;
    Request* req;
    char* data;

    req = malloc(sizeof(Request));
    req->headers = hashmap_create(0);

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
        char* key = trim(line);
        char* value = trim(header_end + 1);
        hashmap_insert(req->headers, key, value);
        line = strtok(NULL, "\r\n");
    }

    free(data);

    return req;
}

char* method_to_string(Method m) {
    switch ((int)m) {
        case 0:
            return "GET";
        case 1:
            return "POST";
        case 2:
            return "PUT";
        case 3:
            return "PATCH";
        case 4:
            return "DELETE";
        case 5:
            return "OPTION";
        default:
            return "UNKNOWN";
    }
}

Method string_to_method(char* input) {
    if (strcmp(input, "GET") == 0) {
        return GET;
    } else if (strcmp(input, "POST") == 0) {
        return POST;
    } else if (strcmp(input, "PUT") == 0) {
        return PUT;
    } else if (strcmp(input, "PATCH") == 0) {
        return PATCH;
    } else if (strcmp(input, "DELETE") == 0) {
        return DELETE;
    } else if (strcmp(input, "OPTION") == 0) {
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

void request_destroy(Request* re) {
    hashmap_destroy(re->headers);
    free((void*)re->url);
    free((void*)re->body);
    free((void*)re->version);
    free(re);
}
