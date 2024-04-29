#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"
typedef struct QueueNode {
    int value;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
    int size;
} Queue;

Queue* queue_create();
void queue_enqueue(Queue*, int);
int queue_dequeue(Queue*);
int queue_is_empty(Queue*);
void queue_print(Queue*);
void queue_destroy(Queue*);
#endif