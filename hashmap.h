#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_INITIAL_SIZE 16
#define LOAD_FACTOR 2


typedef struct Node {
    char* value;
    char* key;
    struct Node* next;
} Node;

typedef struct hashmap {
    unsigned int capacity;
    unsigned int size;
    struct Node** bucket;
} hashmap;

hashmap* hashmap_create();
int hashmap_size(hashmap*);
void hashmap_insert(hashmap*, const char*, const char*);
const char* hashmap_get(hashmap*, const char*);
void hashmap_remove(hashmap*, const char*);
void hashmap_print(hashmap*);
void hashmap_destroy(hashmap* );

#endif