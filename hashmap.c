#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

void remap(hashmap*);
unsigned int find_hash(const char*, unsigned int);

hashmap* hashmap_create() {
    hashmap* map = malloc(sizeof(hashmap));
    map->bucket = malloc(HASHMAP_INITIAL_SIZE * sizeof(Node*));
    map->size = 0;
    map->capacity = HASHMAP_INITIAL_SIZE;
    return map;
}

void hashmap_insert(hashmap* map, const char* key, const char* value) {
    unsigned int position = find_hash(key, map->capacity);
    char* k = malloc(strlen(key));
    char* v = malloc(strlen(value));
    strcpy(v, value);
    strcpy(k, key);

    Node* current = map->bucket[position];
    if (current == NULL) {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->key = k;
        new_node->value = v;
        new_node->next = NULL;
        map->bucket[position] = new_node;
        map->size++;
    } else {
        int found = 0;
        Node* previous = NULL;
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
            Node* new_node = (Node*)malloc(sizeof(Node));
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

void remap(hashmap* map) {
    hashmap* newmap = malloc(sizeof(hashmap));
    newmap->bucket = malloc(map->capacity * 2 * sizeof(Node*));
    newmap->size = 0;
    newmap->capacity = map->capacity * 2;
    for (int i = 0; i < map->capacity; i++) {
        Node* current = map->bucket[i];
        while (current != NULL) {
            hashmap_insert(newmap, current->key, current->value);
            free(current->key);
            free(current->value);
            Node* temp = current->next;
            free(current);
            current = temp;
        }
    }
    free(map->bucket);
    *map = *newmap;
    free(newmap);
}

unsigned int find_hash(const char* key, unsigned int capacity) {
    unsigned int sum = 0;
    for (int i = 0; i < strlen(key); i++) {
        sum += key[i];
    }
    return sum % capacity;
}

void hashmap_print(hashmap* map) {
    for (int i = 0; i < map->capacity; i++) {
        Node* current = map->bucket[i];
        while (current != NULL) {
            printf("%s:%s ", current->key, current->value);
            current = current->next;
        }
        printf("\n");
    }
}

const char* hashmap_get(hashmap* map, const char* key) {
    if (map == NULL) {
        return NULL;
    }
    const unsigned int position = find_hash(key, map->capacity);

    Node* current = map->bucket[position];
    while (current != NULL) {
        if (!strcmp(current->key, key)) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void hashmap_remove(hashmap* map, const char* key) {
    if (map == NULL) {
        return;
    }
    const unsigned int position = find_hash(key, map->capacity);

    Node* current = map->bucket[position];
    Node* previous = NULL;

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

void hashmap_destroy(hashmap* map) {
    if (map == NULL) {
        return;
    }
    for (int i = 0; i < map->capacity; i++) {
        Node* current = map->bucket[i];
        while (current != NULL) {
            free(current->key);
            free(current->value);
            Node* temp = current->next;
            free(current);
            current = temp;
        }
    }
    free(map->bucket);
    free(map);
}
