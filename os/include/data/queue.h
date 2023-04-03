#ifndef QUEUE_H
#define QUEUE_H
#include "process/tcb.h"

typedef struct Node Node_t;

typedef struct Node {
    Node_t* next;
    Node_t* prev;
    Tcb_t* data;
} Node_t;

typedef struct Queue {
    unsigned int size;
    Node_t* head;
    Node_t* tail;
} Queue_t;

void queue_selfcheck(void);
Queue_t* new_queue(void);
void move_node(Queue_t*, Node_t*);
void isolate_node(Queue_t*, Node_t*);
void enqueue_element(Queue_t*, void*);
Node_t* get_head_element(Queue_t*);
Node_t* dequeue_element(Queue_t* queue, Node_t* node);


#endif
