#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_INITIAL_SIZE 16
#define LOAD_FACTOR 2

typedef struct HashmapNode{
    char* value;
    char* key;
    struct HashmapNode* next;
} HashmapNode;

typedef struct Hashmap {
    unsigned int capacity;
    unsigned int size;
    struct HashmapNode** bucket;
} Hashmap;

Hashmap* hashmap_create();
int hashmap_size(Hashmap*);
void hashmap_insert(Hashmap*, const char*, const char*);
const char* hashmap_get(Hashmap*, const char*);
void hashmap_remove(Hashmap*, const char*);
void hashmap_print(Hashmap*);
void hashmap_destroy(Hashmap*);

#endif