#include "data/queue.h"
#include "memory.h"
#include "lang.h"
#include "panic.h"

Queue_t* new_queue(void)
{
  Queue_t* queue = (Queue_t*) allocate(sizeof(Queue_t));
  if (queue == NULL)
  {
    set_panic(OUT_OF_MEMORY);
    return NULL;
  }
  queue->size = 0;
  queue->head = NULL;
  queue->tail = NULL;

  return queue;
}

Node_t* dequeue_element(Queue_t* queue, Node_t* currently_running)
{
  Node_t* q = currently_running;
  if (currently_running == queue->head)
  {
    queue->head = currently_running->next;
    queue->head->prev = queue->tail;
    queue->tail->next = queue->head;

  }
  else if (currently_running == queue->tail)
  {
    queue->tail = currently_running->prev;
    queue->tail->next = queue->head;
    queue->head->prev = queue->tail;

  }
  else
  {
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
    {
      source_queue->head = node->next;
    }
    if (node == source_queue->tail)
    {
      source_queue->tail = node->prev;
    }
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

int enqueue_element(Queue_t* queue, void* data)
{
  Node_t* new_node = (Node_t*) allocate(sizeof(Node_t));

  if (new_node == NULL)
  {
    set_panic(OUT_OF_MEMORY);
    return -1;
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
  return 1;
}