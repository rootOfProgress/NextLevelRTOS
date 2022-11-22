#include "data/queue.h"
#include "memory.h"
#include "lang.h"

Queue_t* new_queue(void)
{
    MemoryResult_t *memory = allocate(sizeof(Queue_t));

    if (memory == NULL)
    {
        // error
    }
    Queue_t* queue = (Queue_t*) memory->start_address;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void insert(Queue_t* queue, void* data)
{
    MemoryResult_t *node_memory = allocate(sizeof(Node_t));

    if (node_memory == NULL)
    {
        // error
    }
    Node_t* new_node = (Node_t*) node_memory->start_address;
    new_node->next = NULL;
    new_node->data = data;

    if (queue->size == 0)
    {
        new_node->next = new_node;
        queue->tail = new_node;
        queue->head = new_node;
    }
    else
    {
        new_node->next = queue->head;
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    queue->size++;
}