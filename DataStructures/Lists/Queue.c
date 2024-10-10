
#include "Queue.h"

void push_queue(struct Queue *queue, void *data, int size);
void pop_queue(struct Queue *queue);
void *front_queue(struct Queue *queue);
int queue_size(struct Queue *queue);

struct Queue queue_constructor(void) {
	struct Queue queue;
	queue.list = linked_list_constructor();
	queue.push = push_queue;
	queue.pop = pop_queue;
	queue.front = front_queue;
	queue.size = queue_size;
	return queue;
}

void queue_destructor(struct Queue *queue) {
	linked_list_destructor(&queue->list);
}

void push_queue(struct Queue *queue, void *data, int size) {
	queue->list.insert(&queue->list, queue->list.length, data, size);
}

void pop_queue(struct Queue *queue) { queue->list.remove(&queue->list, 0); }

void *front_queue(struct Queue *queue) {
	void *data = queue->list.retrieve(&queue->list, 0);
	return data;
}

int queue_size(struct Queue *queue) { return queue->list.length; }
