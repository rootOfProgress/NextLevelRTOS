#ifndef QUEUE_H
#define QUEUE_H

typedef struct Node Node_t;

typedef struct Node {
    Node_t* next;
    Node_t* prev;
    void* data;
} Node_t;

typedef struct Queue {
    unsigned int size;
    Node_t* head;
    Node_t* tail;
} Queue_t;

void queue_selfcheck(void);
Queue_t* new_queue(void);
void enqueue_element(Queue_t*, void*);
Node_t* get_head_element(Queue_t*);
void dequeue_element(Queue_t* queue, Node_t* node);


#endif
