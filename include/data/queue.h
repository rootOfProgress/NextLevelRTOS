#ifndef QUEUE_H
#define QUEUE_H

typedef struct Node Node_t;

typedef struct Node {
    Node_t* next;
    void* data;
} Node_t;

typedef struct Queue {
    unsigned int size;
    Node_t* head;
    Node_t* tail;
} Queue_t;

Queue_t* new_queue(void);
void insert(Queue_t*, void*);

#endif
