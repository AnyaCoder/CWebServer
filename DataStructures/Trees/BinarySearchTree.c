
#include "BinarySearchTree.h"
#include <string.h>

// 获取节点高度
int get_height(void *_n) {
	struct TreeNode *node = (struct TreeNode *)_n;
	if (node == NULL)
		return 0;
	return node->height;
}

// 计算平衡因子
int get_balance(struct TreeNode *node) {
	if (node == NULL)
		return 0;
	return get_height(node->node.prev) - get_height(node->node.next);
}

// 更新节点高度
void update_height(void *_n) {
	struct TreeNode *node = (struct TreeNode *)_n;
	if (node != NULL) {
		int prev_height = get_height(node->node.prev);
		int next_height = get_height(node->node.next);
		node->height =
		    (prev_height > next_height ? prev_height : next_height) + 1;
	}
}

// 右旋
struct TreeNode *next_rotate(struct TreeNode *y) {
	struct TreeNode *x = y->node.prev;
	struct TreeNode *T2 = x->node.next;

	// 旋转
	x->node.next = y;
	y->node.prev = T2;

	// 更新高度
	update_height(y);
	update_height(x);

	return x;
}

// 左旋
struct TreeNode *prev_rotate(struct TreeNode *x) {
	struct TreeNode *y = x->node.next;
	struct TreeNode *T2 = y->node.prev;

	// 旋转
	y->node.prev = x;
	x->node.next = T2;

	// 更新高度
	update_height(x);
	update_height(y);

	return y;
}

// 创建新节点
struct TreeNode *create_node_bst(void *data, int size) {
	struct TreeNode *new_node =
	    (struct TreeNode *)malloc(sizeof(struct TreeNode));
	new_node->node.data = malloc(size);
	memcpy(new_node->node.data, data, size);
	new_node->node.prev = new_node->node.next = NULL;
	new_node->height = 1; // 新节点初始高度为1
	return new_node;
}

// 销毁节点
void destroy_node_bst(struct TreeNode *node_to_destroy) {
	if (node_to_destroy) {
		free(node_to_destroy->node.data);
		free(node_to_destroy);
	}
}

// 递归销毁树
void recursive_tree_destructor(struct TreeNode *node) {
	if (node == NULL)
		return;
	recursive_tree_destructor(node->node.prev);
	recursive_tree_destructor(node->node.next);
	destroy_node_bst(node);
}

// 构造二叉搜索树
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

// 析构二叉搜索树
void binary_search_tree_destructor(struct BinarySearchTree *tree) {
	recursive_tree_destructor(tree->head);
	tree->head = NULL;
}

// 搜索操作
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

// 插入操作，并返回新的子树根
struct TreeNode *insert_node_avl(struct BinarySearchTree *tree,
                                 struct TreeNode *node, void *data, int size) {
	// 1. 普通的BST插入
	if (node == NULL)
		return create_node_bst(data, size);

	int cmp = tree->compare(data, node->node.data);
	if (cmp < 0)
		node->node.prev = insert_node_avl(tree, node->node.prev, data, size);
	else if (cmp > 0)
		node->node.next = insert_node_avl(tree, node->node.next, data, size);
	else // 重复数据不插入
		return node;

	// 2. 更新高度
	update_height(node);

	// 3. 获取平衡因子
	int balance = get_balance(node);

	// 4. 根据平衡因子进行旋转

	// 左左情况
	if (balance > 1 && tree->compare(data, node->node.prev->data) < 0)
		return next_rotate(node);

	// 右右情况
	if (balance < -1 && tree->compare(data, node->node.next->data) > 0)
		return prev_rotate(node);

	// 左右情况
	if (balance > 1 && tree->compare(data, node->node.prev->data) > 0) {
		node->node.prev = prev_rotate(node->node.prev);
		return next_rotate(node);
	}

	// 右左情况
	if (balance < -1 && tree->compare(data, node->node.next->data) < 0) {
		node->node.next = next_rotate(node->node.next);
		return prev_rotate(node);
	}

	// 返回节点指针
	return node;
}

// 插入操作
void insert_bst(struct BinarySearchTree *tree, void *data, int size) {
	tree->head = insert_node_avl(tree, tree->head, data, size);
}

// 中序遍历
void in_order_traversal(struct TreeNode *node, void (*visit)(void *data)) {
	if (node == NULL)
		return;

	// 访问左子树
	in_order_traversal(node->node.prev, visit);

	// 访问当前节点
	visit(node->node.data);

	// 访问右子树
	in_order_traversal(node->node.next, visit);
}

// 遍历整棵树
void traverse_bst(struct BinarySearchTree *tree, void (*visit)(void *data)) {
	if (tree == NULL || tree->head == NULL)
		return;
	in_order_traversal(tree->head, visit);
}

struct TreeNode *find_minimum_node(void *node);

// 删除节点，并返回新的子树根
struct Node *remove_node_avl(struct BinarySearchTree *tree,
                             struct TreeNode *node, void *data, int data_size) {
	// 标准BST删除过程
	if (node == NULL)
		return node;

	int cmp = tree->compare(data, node->node.data);
	if (cmp < 0) {
		// 要删除的数据在左子树
		node->node.prev =
		    remove_node_avl(tree, node->node.prev, data, data_size);
	} else if (cmp > 0) {
		// 要删除的数据在右子树
		node->node.next =
		    remove_node_avl(tree, node->node.next, data, data_size);
	} else {
		// 找到要删除的节点
		if (node->node.prev == NULL || node->node.next == NULL) {
			// 一个或零个子节点
			struct TreeNode *temp =
			    node->node.prev ? node->node.prev : node->node.next;

			if (temp == NULL) {
				// 没有子节点（叶子节点）
				temp = node;
				node = NULL;
			} else {
				// 一个子节点
				*node = *temp; // 复制子节点的内容到当前节点
			}
			destroy_node_bst(temp);
		} else {
			// 有两个子节点，找到右子树中的最小节点（中序后继）
			struct TreeNode *temp = find_minimum_node(node->node.next);

			// 复制中序后继的数据到当前节点
			memcpy(node->node.data, temp->node.data, data_size);

			// 删除中序后继节点
			node->node.next = remove_node_avl(tree, node->node.next,
			                                  temp->node.data, data_size);
		}
	}

	// 如果树只有一个节点，返回
	if (node == NULL)
		return node;

	// 更新高度
	update_height(node);

	// 重新平衡AVL树
	int balance = get_balance(node);

	// 左左情况
	if (balance > 1 && get_balance(node->node.prev) >= 0)
		return next_rotate(node);

	// 左右情况
	if (balance > 1 && get_balance(node->node.prev) < 0) {
		node->node.prev = prev_rotate(node->node.prev);
		return next_rotate(node);
	}

	// 右右情况
	if (balance < -1 && get_balance(node->node.next) <= 0)
		return prev_rotate(node);

	// 右左情况
	if (balance < -1 && get_balance(node->node.next) > 0) {
		node->node.next = next_rotate(node->node.next);
		return prev_rotate(node);
	}

	return node;
}

// 辅助函数，找到最小值节点
struct TreeNode *find_minimum_node(void *node) {
	struct Node *current = (struct Node *)node;
	while (current->prev != NULL)
		current = current->prev;
	return (struct TreeNode *)(void *)current;
}

// 删除函数
void remove_bst(struct BinarySearchTree *tree, void *data, int size) {
	tree->head = remove_node_avl(tree, tree->head, data, size);
}
