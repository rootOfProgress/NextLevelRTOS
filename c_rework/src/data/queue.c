#include "data/queue.h"
#include "memory.h"
#include "lang.h"

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


// @ todo: does not work yet
void dequeue_element(Queue_t* queue, Node_t* node)
{
    node->prev->next = node->next;
    node->prev = NULL;
    node->next = NULL;

    // @todo needs tests
    //deallocate((void*) node);
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