#include "data/list.h"
#include "memory.h"
#include "lang.h"
#include "panic.h"

List_t* new_list(void)
{
  List_t* list = (List_t*) allocate(sizeof(List_t));
  if (list == NULL)
    invoke_panic(OUT_OF_MEMORY);

  list->size = 0;
  list->head = NULL;

  return list;
}

void single_list_push(List_t* list, void* data)
{
  SingleLinkedNode_t* new_node = (SingleLinkedNode_t*) allocate(sizeof(SingleLinkedNode_t));

  if (new_node == NULL)
  {
    invoke_panic(OUT_OF_MEMORY);
  }

  new_node->next = NULL;
  new_node->data = data;

  if (list->size == 0)
  {
    list->head = new_node;
  }
  else
  {
    SingleLinkedNode_t* q = list->head;
    while (q->next != NULL)
    {
      q = q->next;
    }
    q->next = new_node;
  }
  list->size++;
}

SingleLinkedNode_t* single_list_pop(List_t* list)
{
  if (list->size == 0)
    return NULL;
  SingleLinkedNode_t* requested_node = list->head;
  list->head = list->head->next;
  list->size--;
  return requested_node;
}