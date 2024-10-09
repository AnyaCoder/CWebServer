
#include "LinkedList.h"

void bubblesort_linklist(struct LinkedList* linked_list, int (*compare)(void* a, void* b));

struct LinkedList linked_list_constructor(void)
{
	struct LinkedList new_list;
	new_list.head = NULL;
	new_list.length = 0;

	new_list.insert = insert_node_linklist;
	new_list.remove = remove_node_linklist;
	new_list.retrieve = retrieve_data_linklist;
	new_list.sort = bubblesort_linklist;

	return new_list;
};

void linked_list_destructor(struct LinkedList* linked_list)
{
	while (linked_list->length > 0)
	{
		linked_list->remove(linked_list, 0);
	}
}

struct Node* create_node_linklist(void* data, int size)
{
	struct Node* new_node = 
		(struct Node*)malloc(sizeof(struct Node));
	*new_node = node_constructor(data, size);
	return new_node;
};

void destroy_node_linklist(struct Node* node_to_destroy)
{
	node_destructor(node_to_destroy);
}

struct Node* iterate_linklist(struct LinkedList* linked_list, int index)
{
	if (index < 0 || index >= linked_list->length)
	{
		perror("Index out of bound");
		exit(9);
	}
	struct Node* cursor = linked_list->head;
	for (int i = 0; i < index; i++)
	{
		cursor = cursor->next;
	}
	return cursor;
}

void insert_node_linklist(struct LinkedList* linked_list, int index, void* data, int size)
{
	struct Node* node_to_insert = create_node_linklist(data, size);
	if (index == 0)
	{
		node_to_insert->next = linked_list->head;
		linked_list->head = node_to_insert;
	}
	else
	{
		struct Node* cursor = iterate_linklist(linked_list, index - 1);
		node_to_insert->next = cursor->next;
		cursor->next = node_to_insert;
	}
	linked_list->length += 1;
}

void remove_node_linklist(struct LinkedList* linked_list, int index)
{
	if (index == 0)
	{
		struct Node* node_to_remove = linked_list->head;
		linked_list->head = node_to_remove->next;
		destroy_node_linklist(node_to_remove);
	}
	else
	{
		struct Node* cursor = iterate_linklist(linked_list, index - 1);
		struct Node* node_to_remove = cursor->next;
		cursor->next = node_to_remove->next;
		destroy_node_linklist(node_to_remove);
	}
	linked_list->length -= 1;
}

void* retrieve_data_linklist(struct LinkedList* linked_list, int index)
{
	struct Node* cursor = iterate_linklist(linked_list, index);
	if (cursor)
	{
		return cursor->data;
	}
	else
	{
		return NULL;
	}

}

void bubblesort_linklist(struct LinkedList* linked_list, int (*compare)(void* a, void* b))
{	
	struct Node* cursor_i = linked_list->head;
	
	for (int i = 0; i < linked_list->length; i++)
	{
		struct Node* cursor_j = cursor_i->next;
		
		for (int j = i + 1; j < linked_list->length; j++)
		{
			if (compare(cursor_i->data, cursor_j->data) == 1)
			{
				void* temp = cursor_j->data;
				cursor_j->data = cursor_i->data;
				cursor_i->data = temp;
			}
			cursor_j = cursor_j->next;
		}
		cursor_i = cursor_i->next;
	}
}