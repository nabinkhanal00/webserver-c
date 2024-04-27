#include <stdlib.h>

#include "queue.h"

Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        err_n_die("memory allocation failed while creating queue.");
    }
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;

    return q;
}

void enqueue(Queue* q, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        err_n_die("memory allocation failed while creating node.");
    }
    newNode->value = value;
    newNode->next = NULL;
    if (is_empty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->size++;
}

int dequeue(Queue* q) {
    if (is_empty(q)) {
        err_n_die("trying to dequeue from empty queue.");
    }
    Node* temp = q->front;
    int value = temp->value;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    q->size--;
    return value;
}

int is_empty(Queue* q) { return q->front == NULL; }

void print_queue(Queue* q) {
    if (q == NULL) {
        return;
    }
    while (q->front != NULL) {
        q->front = q->front->next;
    }
}
