#ifndef Node_h
#define Node_h

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

struct Node
{
	void* data;
	struct Node* prev;
	struct Node* next;
};

struct Node node_constructor(void* data, int size);
void node_destructor(struct Node* node);

#endif // !Node_h