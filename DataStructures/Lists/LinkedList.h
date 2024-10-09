
#ifndef LinkedList_h
#define LinkedList_h

#include "../Common/Node.h"
#include <stdio.h>
#include <stdlib.h>

struct LinkedList
{
	struct Node* head;
	int length;

	void (*insert)(struct LinkedList* linked_list, int index, void* data, int size);
	void (*remove)(struct LinkedList* linked_list, int index);
	void* (*retrieve)(struct LinkedList* linked_list, int index);
	void (*sort)(struct LinkedList* linked_list, int (*compare)(void* a, void* b));
};

struct LinkedList linked_list_constructor(void);

void linked_list_destructor(struct LinkedList* linked_list);

struct Node* create_node_linklist(void* data, int size);

void destroy_node_linklist(struct Node* node_to_destroy);

struct Node* iterate_linklist(struct LinkedList* linked_list, int index);

void insert_node_linklist(struct LinkedList* linked_list, int index, void* data, int size);

void remove_node_linklist(struct LinkedList* linked_list, int index);

void* retrieve_data_linklist(struct LinkedList* linked_list, int index);

#endif // !LinkedList_h