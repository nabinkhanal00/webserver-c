#include <stdlib.h>

#include "queue.h"

Queue* queue_create() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        err_n_die("memory allocation failed while creating queue.");
    }
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;

    return q;
}

void queue_enqueue(Queue* q, int value) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_node == NULL) {
        err_n_die("memory allocation failed while creating node.");
    }
    new_node->value = value;
    new_node->next = NULL;
    if (queue_is_empty(q)) {
        q->front = new_node;
        q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
    q->size++;
}

int queue_dequeue(Queue* q) {
    if (queue_is_empty(q)) {
        err_n_die("trying to dequeue from empty queue.");
    }
    QueueNode* temp = q->front;
    int value = temp->value;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    q->size--;
    return value;
}

int queue_is_empty(Queue* q) { return q->front == NULL; }

void queue_print(Queue* q) {
    if (q == NULL) {
        return;
    }
    while (q->front != NULL) {
        q->front = q->front->next;
    }
}
