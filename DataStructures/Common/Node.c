
#include "Node.h"

#include <string.h>

struct Node node_constructor(void *data, int size) {
	if (size < 1) {
		printf("Invalid data size for node...\n");
		exit(1);
	}
	struct Node node;
	node.data = malloc(size);
	memcpy(node.data, data, size);
	node.prev = NULL;
	node.next = NULL;
	return node;
}

void node_destructor(struct Node *node) {
	free(node->data);
	free(node);
}
