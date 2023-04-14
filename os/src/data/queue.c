#include "data/queue.h"
#include "memory.h"
#include "lang.h"
#include "panic.h"

#ifdef SELF_CHECK
void queue_selfcheck(void)
{
    unsigned int test_a = 1;
    unsigned int test_b = 1;
    unsigned int test_c = 1;
    unsigned int test_d = 1;
    unsigned int test_e = 1;
    unsigned int test_f = 1;
    unsigned int test_g = 1;
    unsigned int test_h = 1;
    unsigned int test_i = 1;
    unsigned int test_j = 1;
    unsigned int test_k = 1;

    Queue_t* q1 = new_queue();

    // test a: test init
    if (!q1)
        test_a = 0;  

    // test b: test insert
    enqueue_element(q1, 1);
    enqueue_element(q1, 2);
    enqueue_element(q1, 3);
    enqueue_element(q1, 4);
    enqueue_element(q1, 5);
    if (q1->size != 5)
        test_b = 0;  

    // test c: check order forward
    Node_t *p = q1->head;
    for (unsigned int i = 0; i < q1->size; i++) 
    {
        if (p->data != i+1)
            test_c = 0;
        p = p->next;
    }

    // test d: check order backward
    p = q1->tail;
    for (unsigned int i = q1->size; i > 0; i--) 
    {
        if (p->data != i)
            test_d = 0;
        p = p->prev;
    }

    // test e: check tail / head
    if (q1->head->prev != q1->tail || q1->tail->next != q1->head)
        test_e = 0;

      
}
#endif
Queue_t* new_queue(void)
{
    Queue_t* queue = (Queue_t*) allocate(sizeof(Queue_t));
    if (queue == NULL)
        invoke_panic(OUT_OF_MEMORY);
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    
    return queue;
}


// @ todo: does not work yet?
Node_t* dequeue_element(Queue_t* queue, Node_t* currently_running)
{
    Node_t* q = currently_running;
    if (currently_running == queue->head)
    {
        queue->head = currently_running->next;
        queue->head->prev = queue->tail;
        queue->tail->next = queue->head;

    } else if (currently_running == queue->tail)
    {
        queue->tail = currently_running->prev;
        queue->tail->next = queue->head;
        queue->head->prev = queue->tail;

    } else {
        currently_running->prev->next = currently_running->next;
        currently_running->next->prev = currently_running->prev;
    }
    queue->size--;
    return q;
}

Node_t* get_head_element(Queue_t* queue)
{
    return queue->head;
}

void isolate_node(Queue_t* source_queue, Node_t* node)
{
    source_queue->size--;
    if (source_queue->size == 0)
    {
        source_queue->head = NULL;
        source_queue->tail = NULL;
    }
    else
    {
        if (node == source_queue->head)
            source_queue->head = node->next;
        if (node == source_queue->tail)
            source_queue->tail = node->prev;
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
}


void move_node(Queue_t* target_queue, Node_t* node)
{
    if (target_queue->size == 0)
    {
        node->next = node;
        node->prev = node;
        target_queue->tail = node;
        target_queue->head = node;
    }
    else
    {
        // @todo: update head nodes prev to tail!
        node->prev = target_queue->tail;
        node->next = target_queue->head;
        target_queue->tail->next = node;
        target_queue->tail = node;
        target_queue->head->prev = target_queue->tail;
    }
    target_queue->size++;
}

void enqueue_element(Queue_t* queue, void* data)
{
    Node_t* new_node = (Node_t*) allocate(sizeof(Node_t));

    if (new_node == NULL)
        invoke_panic(OUT_OF_MEMORY);
        
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