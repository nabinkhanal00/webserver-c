#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "hashmap.h"

void remap(Hashmap*);
unsigned int find_hash(const char*, unsigned int);

Hashmap* hashmap_create(unsigned int element_size) {
    Hashmap* map = malloc(sizeof(Hashmap));
    map->element_size = element_size;
    if (map == NULL) {
        err_n_die("Memory allocation failed while creating map.");
    }
    map->bucket = malloc(HASHMAP_INITIAL_SIZE * sizeof(HashmapNode*));
    if (map->bucket == NULL) {
        err_n_die("Memory allocation failed while creating bucket.");
    }
    map->size = 0;
    map->capacity = HASHMAP_INITIAL_SIZE;
    return map;
}

void hashmap_insert(Hashmap* map, const char* key, void* value) {
    unsigned int position = find_hash(key, map->capacity);
    char* k = malloc(strlen(key));
    strcpy(k, key);
    void* v;
    if (map->element_size == -1) {
        v = value;
    } else if (map->element_size == 0) {
        v = malloc(strlen(value));
        strcpy(v, value);
    } else {
        v = malloc(map->element_size);
        memcpy(v, value, map->element_size);
    }
    if (k == NULL || v == NULL) {
        err_n_die("Memory allocation failed while copying key and value.");
    }

    HashmapNode* current = map->bucket[position];
    if (current == NULL) {
        HashmapNode* new_node = (HashmapNode*)malloc(sizeof(HashmapNode));
        if (new_node == NULL) {
            err_n_die("Memory allocation failed while creating node.");
        }
        new_node->key = k;
        new_node->value = v;
        new_node->next = NULL;
        map->bucket[position] = new_node;
        map->size++;
    } else {
        int found = 0;
        HashmapNode* previous = NULL;
        while (current != NULL) {
            if (!strcmp(current->key, key)) {
                found = 1;
                free(current->value);
                free(k);
                current->value = v;
                break;
            } else {
                previous = current;
                current = current->next;
            }
        }
        if (!found) {
            HashmapNode* new_node = (HashmapNode*)malloc(sizeof(HashmapNode));
            if (new_node == NULL) {
                err_n_die("Memory allocation failed while creating node.");
            }
            new_node->key = k;
            new_node->value = v;
            new_node->next = NULL;
            previous->next = new_node;
            map->size++;
        }
    }

    if (map->size * LOAD_FACTOR > map->capacity) {
        remap(map);
    }
}

void remap(Hashmap* map) {
    Hashmap* new_map = malloc(sizeof(Hashmap));
    if (new_map == NULL) {
        err_n_die("Memory allocation failed while creating map.");
    }
    new_map->bucket = malloc(map->capacity * 2 * sizeof(HashmapNode*));
    if (new_map->bucket == NULL) {
        err_n_die("Memory allocation failed while creating bucket.");
    }
    new_map->size = 0;
    new_map->capacity = map->capacity * 2;
    for (unsigned int i = 0; i < map->capacity; i++) {
        HashmapNode* current = map->bucket[i];
        while (current != NULL) {
            hashmap_insert(new_map, current->key, current->value);
            free(current->key);
            free(current->value);
            HashmapNode* temp = current->next;
            free(current);
            current = temp;
        }
    }
    free(map->bucket);
    *map = *new_map;
    free(new_map);
}

unsigned int find_hash(const char* key, unsigned int capacity) {
    unsigned int sum = 0;
    for (size_t i = 0; i < strlen(key); i++) {
        sum += key[i];
    }
    return sum % capacity;
}

void hashmap_print(Hashmap* map) {
    for (unsigned int i = 0; i < map->capacity; i++) {
        HashmapNode* current = map->bucket[i];
        if (map->element_size == 0) {
            while (current != NULL) {
                printf("%s:%s ", current->key, (char*)current->value);
                current = current->next;
            }
        } else {
            while (current != NULL) {
                printf("%s:%p ", current->key, current->value);
                current = current->next;
            }
        }
        printf("\n");
    }
}

void* hashmap_get(Hashmap* map, const char* key) {
    if (map == NULL) {
        return NULL;
    }
    const unsigned int position = find_hash(key, map->capacity);

    HashmapNode* current = map->bucket[position];
    while (current != NULL) {
        if (!strcmp(current->key, key)) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void hashmap_remove(Hashmap* map, const char* key) {
    if (map == NULL) {
        return;
    }
    const unsigned int position = find_hash(key, map->capacity);

    HashmapNode* current = map->bucket[position];
    HashmapNode* previous = NULL;

    while (current != NULL) {
        if (!strcmp(current->key, key)) {
            free(current->key);
            free(current->value);
            if (previous == NULL) {
                map->bucket[position] = NULL;
            } else {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void hashmap_destroy(Hashmap* map) {
    if (map == NULL) {
        return;
    }
    for (unsigned int i = 0; i < map->capacity; i++) {
        HashmapNode* current = map->bucket[i];
        while (current != NULL) {
            free(current->key);
            free(current->value);
            HashmapNode* temp = current->next;
            free(current);
            current = temp;
        }
    }
    free(map->bucket);
    free(map);
}
