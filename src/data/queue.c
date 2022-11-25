#include "data/queue.h"
#include "memory.h"
#include "lang.h"

Queue_t* new_queue(void)
{
    Queue_t* queue = (Queue_t*) allocate(sizeof(Queue_t));
    if (queue == NULL)
    {
        // error
    }
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    
    return queue;
}

void dequeue_element(Queue_t* queue, Node_t* node)
{
    node->prev->next = node->next;
    node->prev = NULL;
    node->next = NULL;

    // @todo needs tests
    deallocate((void*) node);
    queue->size--;
}

Node_t* get_head_element(Queue_t* queue)
{
    return queue->head;
}

void enqueue_element(Queue_t* queue, void* data)
{
    Node_t* new_node = (Node_t*) allocate(sizeof(Node_t));
    if (new_node == NULL)
    {
        // error
    }
    new_node->next = NULL;
    new_node->data = data;

    if (queue->size == 0)
    {
        new_node->next = new_node;
        new_node->prev = new_node;
        queue->tail = new_node;
        queue->head = new_node;
    }
    else
    {
        // @todo: update head nodes prev to tail!
        new_node->prev = queue->tail;
        new_node->next = queue->head;
        queue->tail->next = new_node;
        queue->tail = new_node;
        queue->head->prev = queue->tail;
    }
    queue->size++;
}