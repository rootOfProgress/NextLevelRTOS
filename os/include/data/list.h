#ifndef LIST_H
#define LIST_H

typedef struct SingleNode SingleLinkedNode_t;

typedef struct SingleNode {
    SingleLinkedNode_t* next;
    void* data;
} SingleLinkedNode_t;

typedef struct List {
    int size;
    SingleLinkedNode_t* head;
} List_t;

List_t* new_list(void);

void single_list_push(List_t*, void*);
SingleLinkedNode_t* single_list_pop(List_t*);

#endif
