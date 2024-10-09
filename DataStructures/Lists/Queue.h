
#ifndef Queue_h
#define Queue_h

#include "LinkedList.h"

struct Queue
{
	struct LinkedList list;

	void (*push)(struct Queue* queue, void* data, int size);
	void (*pop)(struct Queue* queue);
	void* (*front)(struct Queue* queue);
	int (*size)(struct Queue* queue);

};

struct Queue queue_constructor(void);
void queue_destructor(struct Queue* queue);

#endif // !Queue_h