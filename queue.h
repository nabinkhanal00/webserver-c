#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"
typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
    int size;
} Queue;

Queue* create_queue();
void enqueue(Queue*, int);
int dequeue(Queue*);
int is_empty(Queue*);
void print_queue(Queue*);
#endif