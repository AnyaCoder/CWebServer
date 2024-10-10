#ifndef Dictionary_h
#define Dictionary_h

#include "../Trees/BinarySearchTree.h"
#include "Entry.h"

struct Dictionary {
	struct BinarySearchTree binary_search_tree;

	void (*insert)(struct Dictionary *dictionary, void *key, int key_size,
	               void *value, int value_size);
	void *(*search)(struct Dictionary *dictionary, void *key, int key_size);
};

struct Dictionary dictionary_constructor(int (*compare)(void *key_1,
                                                        void *key_2));
void dictionary_destructor(struct Dictionary *dictionary);
int compare_string_keys(void *entry_1, void *entry_2);
#endif // !Dictionary_h
