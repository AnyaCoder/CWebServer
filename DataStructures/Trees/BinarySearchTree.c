
#include "BinarySearchTree.h"
#include <string.h>

// ��ȡ�ڵ�߶�
int get_height(void *_n) {
	struct TreeNode *node = (struct TreeNode *)_n;
	if (node == NULL)
		return 0;
	return node->height;
}

// ����ƽ������
int get_balance(struct TreeNode *node) {
	if (node == NULL)
		return 0;
	return get_height(node->node.prev) - get_height(node->node.next);
}

// ���½ڵ�߶�
void update_height(void *_n) {
	struct TreeNode *node = (struct TreeNode *)_n;
	if (node != NULL) {
		int prev_height = get_height(node->node.prev);
		int next_height = get_height(node->node.next);
		node->height =
		    (prev_height > next_height ? prev_height : next_height) + 1;
	}
}

// ����
struct TreeNode *next_rotate(struct TreeNode *y) {
	struct TreeNode *x = y->node.prev;
	struct TreeNode *T2 = x->node.next;

	// ��ת
	x->node.next = y;
	y->node.prev = T2;

	// ���¸߶�
	update_height(y);
	update_height(x);

	return x;
}

// ����
struct TreeNode *prev_rotate(struct TreeNode *x) {
	struct TreeNode *y = x->node.next;
	struct TreeNode *T2 = y->node.prev;

	// ��ת
	y->node.prev = x;
	x->node.next = T2;

	// ���¸߶�
	update_height(x);
	update_height(y);

	return y;
}

// �����½ڵ�
struct TreeNode *create_node_bst(void *data, int size) {
	struct TreeNode *new_node =
	    (struct TreeNode *)malloc(sizeof(struct TreeNode));
	new_node->node.data = malloc(size);
	memcpy(new_node->node.data, data, size);
	new_node->node.prev = new_node->node.next = NULL;
	new_node->height = 1; // �½ڵ��ʼ�߶�Ϊ1
	return new_node;
}

// ���ٽڵ�
void destroy_node_bst(struct TreeNode *node_to_destroy) {
	if (node_to_destroy) {
		free(node_to_destroy->node.data);
		free(node_to_destroy);
	}
}

// �ݹ�������
void recursive_tree_destructor(struct TreeNode *node) {
	if (node == NULL)
		return;
	recursive_tree_destructor(node->node.prev);
	recursive_tree_destructor(node->node.next);
	destroy_node_bst(node);
}

// �������������
struct BinarySearchTree
binary_search_tree_constructor(int (*compare)(void *data_1, void *data_2)) {
	struct BinarySearchTree tree;
	tree.head = NULL;
	tree.compare = compare;
	tree.search = search_bst;
	tree.insert = insert_bst;
	tree.destroy = binary_search_tree_destructor;
	return tree;
}

// ��������������
void binary_search_tree_destructor(struct BinarySearchTree *tree) {
	recursive_tree_destructor(tree->head);
	tree->head = NULL;
}

// ��������
void *search_bst(struct BinarySearchTree *tree, void *data, int data_size) {
	struct TreeNode *current = tree->head;
	while (current != NULL) {
		int cmp = tree->compare(data, current->node.data);
		if (cmp == 0) {
			return current->node.data;
		} else if (cmp < 0) {
			current = current->node.prev;
		} else {
			current = current->node.next;
		}
	}
	return NULL;
}

// ����������������µ�������
struct TreeNode *insert_node_avl(struct BinarySearchTree *tree,
                                 struct TreeNode *node, void *data, int size) {
	// 1. ��ͨ��BST����
	if (node == NULL)
		return create_node_bst(data, size);

	int cmp = tree->compare(data, node->node.data);
	if (cmp < 0)
		node->node.prev = insert_node_avl(tree, node->node.prev, data, size);
	else if (cmp > 0)
		node->node.next = insert_node_avl(tree, node->node.next, data, size);
	else // �ظ����ݲ�����
		return node;

	// 2. ���¸߶�
	update_height(node);

	// 3. ��ȡƽ������
	int balance = get_balance(node);

	// 4. ����ƽ�����ӽ�����ת

	// �������
	if (balance > 1 && tree->compare(data, node->node.prev->data) < 0)
		return next_rotate(node);

	// �������
	if (balance < -1 && tree->compare(data, node->node.next->data) > 0)
		return prev_rotate(node);

	// �������
	if (balance > 1 && tree->compare(data, node->node.prev->data) > 0) {
		node->node.prev = prev_rotate(node->node.prev);
		return next_rotate(node);
	}

	// �������
	if (balance < -1 && tree->compare(data, node->node.next->data) < 0) {
		node->node.next = next_rotate(node->node.next);
		return prev_rotate(node);
	}

	// ���ؽڵ�ָ��
	return node;
}

// �������
void insert_bst(struct BinarySearchTree *tree, void *data, int size) {
	tree->head = insert_node_avl(tree, tree->head, data, size);
}

// �������
void in_order_traversal(struct TreeNode *node, void (*visit)(void *data)) {
	if (node == NULL)
		return;

	// ����������
	in_order_traversal(node->node.prev, visit);

	// ���ʵ�ǰ�ڵ�
	visit(node->node.data);

	// ����������
	in_order_traversal(node->node.next, visit);
}

// ����������
void traverse_bst(struct BinarySearchTree *tree, void (*visit)(void *data)) {
	if (tree == NULL || tree->head == NULL)
		return;
	in_order_traversal(tree->head, visit);
}

struct TreeNode *find_minimum_node(void *node);

// ɾ���ڵ㣬�������µ�������
struct Node *remove_node_avl(struct BinarySearchTree *tree,
                             struct TreeNode *node, void *data, int data_size) {
	// ��׼BSTɾ������
	if (node == NULL)
		return node;

	int cmp = tree->compare(data, node->node.data);
	if (cmp < 0) {
		// Ҫɾ����������������
		node->node.prev =
		    remove_node_avl(tree, node->node.prev, data, data_size);
	} else if (cmp > 0) {
		// Ҫɾ����������������
		node->node.next =
		    remove_node_avl(tree, node->node.next, data, data_size);
	} else {
		// �ҵ�Ҫɾ���Ľڵ�
		if (node->node.prev == NULL || node->node.next == NULL) {
			// һ��������ӽڵ�
			struct TreeNode *temp =
			    node->node.prev ? node->node.prev : node->node.next;

			if (temp == NULL) {
				// û���ӽڵ㣨Ҷ�ӽڵ㣩
				temp = node;
				node = NULL;
			} else {
				// һ���ӽڵ�
				*node = *temp; // �����ӽڵ�����ݵ���ǰ�ڵ�
			}
			destroy_node_bst(temp);
		} else {
			// �������ӽڵ㣬�ҵ��������е���С�ڵ㣨�����̣�
			struct TreeNode *temp = find_minimum_node(node->node.next);

			// ���������̵����ݵ���ǰ�ڵ�
			memcpy(node->node.data, temp->node.data, data_size);

			// ɾ�������̽ڵ�
			node->node.next = remove_node_avl(tree, node->node.next,
			                                  temp->node.data, data_size);
		}
	}

	// �����ֻ��һ���ڵ㣬����
	if (node == NULL)
		return node;

	// ���¸߶�
	update_height(node);

	// ����ƽ��AVL��
	int balance = get_balance(node);

	// �������
	if (balance > 1 && get_balance(node->node.prev) >= 0)
		return next_rotate(node);

	// �������
	if (balance > 1 && get_balance(node->node.prev) < 0) {
		node->node.prev = prev_rotate(node->node.prev);
		return next_rotate(node);
	}

	// �������
	if (balance < -1 && get_balance(node->node.next) <= 0)
		return prev_rotate(node);

	// �������
	if (balance < -1 && get_balance(node->node.next) > 0) {
		node->node.next = next_rotate(node->node.next);
		return prev_rotate(node);
	}

	return node;
}

// �����������ҵ���Сֵ�ڵ�
struct TreeNode *find_minimum_node(void *node) {
	struct Node *current = (struct Node *)node;
	while (current->prev != NULL)
		current = current->prev;
	return (struct TreeNode *)(void *)current;
}

// ɾ������
void remove_bst(struct BinarySearchTree *tree, void *data, int size) {
	tree->head = remove_node_avl(tree, tree->head, data, size);
}
