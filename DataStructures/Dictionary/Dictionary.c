
#include "Dictionary.h"
#include <string.h>

void insert_dict(struct Dictionary *dictionary, void *key, int key_size,
                 void *value, int value_size);
void *search_dict(struct Dictionary *dictionary, void *key, int key_size);

struct Dictionary dictionary_constructor(int (*compare)(void *key_1,
                                                        void *key_2)) {
	struct Dictionary dictionary;
	dictionary.binary_search_tree = binary_search_tree_constructor(compare);
	dictionary.insert = insert_dict;
	dictionary.search = search_dict;
	return dictionary;
}

void dictionary_destructor(struct Dictionary *dictionary) {
	binary_search_tree_destructor(&dictionary->binary_search_tree);
}

void *search_dict(struct Dictionary *dictionary, void *key, int key_size) {
	int dummy_value = 0;
	struct Entry searchable =
	    entry_constructor(key, key_size, &dummy_value, sizeof(dummy_value));
	void *result = dictionary->binary_search_tree.search(
	    &dictionary->binary_search_tree, &searchable, sizeof(searchable));
	if (result) {
		return ((struct Entry *)result)->value;
	} else {
		return NULL;
	}
}

void insert_dict(struct Dictionary *dictionary, void *key, int key_size,
                 void *value, int value_size) {
	struct Entry entry = entry_constructor(key, key_size, value, value_size);

	dictionary->binary_search_tree.insert(&dictionary->binary_search_tree,
	                                      &entry, sizeof(entry));
}

// MARK: PUBLIC HELPER FUNCTIONS
int compare_string_keys(void *entry_1, void *entry_2) {
	if (strcmp((char *)((struct Entry *)entry_1)->key,
	           (char *)((struct Entry *)entry_2)->key) > 0) {
		return 1;
	} else if (strcmp((char *)((struct Entry *)entry_1)->key,
	                  (char *)((struct Entry *)entry_2)->key) < 0) {
		return -1;
	} else {
		return 0;
	}
}
