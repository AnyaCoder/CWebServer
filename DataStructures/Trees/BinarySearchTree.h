#ifndef BinarySearchTree_h
#define BinarySearchTree_h

#include "../Common/Node.h"

struct TreeNode {
    struct Node node;
    int height; // 新增字段，用于AVL树的平衡
};

struct BinarySearchTree {
    struct Node* head;
    int (*compare)(void* data_1, void* data_2);
    void* (*search)(struct BinarySearchTree* tree, void* data, int data_size);
    void (*insert)(struct BinarySearchTree* tree, void* data, int size);
    void (*destroy)(struct BinarySearchTree* tree);
};

// 函数声明
struct BinarySearchTree binary_search_tree_constructor(
    int (*compare)(void* data_1, void* data_2)
);
void binary_search_tree_destructor(struct BinarySearchTree* tree);
void* search_bst(struct BinarySearchTree* tree, void* data, int data_size);
void insert_bst(struct BinarySearchTree* tree, void* data, int size);
void traverse_bst(struct BinarySearchTree* tree, void (*visit)(void* data));
void remove_bst(struct BinarySearchTree* tree, void* data, int size);
#endif