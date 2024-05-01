#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_INITIAL_SIZE 8
#define LOAD_FACTOR 2

typedef struct HashmapNode{
    void* value;
    char* key;
    struct HashmapNode* next;
} HashmapNode;

typedef struct Hashmap {
    unsigned int capacity;
    unsigned int size;

    // represents different types of elements
    // -1 for function pointers
    // 0 for const char* or char*
    // <size of the element> using function (sizeof) for any other elements
    int element_size;
    struct HashmapNode** bucket;
} Hashmap;

Hashmap* hashmap_create(unsigned int);
int hashmap_size(Hashmap*);
void hashmap_insert(Hashmap*, const char*, void*);
void* hashmap_get(Hashmap*, const char*);
void hashmap_remove(Hashmap*, const char*);
void hashmap_print(Hashmap*);
void hashmap_destroy(Hashmap*);

#endif