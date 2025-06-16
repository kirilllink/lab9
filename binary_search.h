#include "service.h"
#include <stdlib.h>
#include <string.h>

typedef struct BinaryNode {
    Service *data;
    struct BinaryNode *left;
    struct BinaryNode *right;
} BinaryNode;

// Создание нового узла Binary
BinaryNode* create_node_binary(Service *data) {
    BinaryNode *node = (BinaryNode*)malloc(sizeof(BinaryNode));
    if (!node) exit(1);
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

BinaryNode* Binary_insert(BinaryNode **root, Service *data) {
    if (*root == NULL) {
        *root = create_node_binary(data);
        return *root;
    }
    if (strcmp(data->service_name, (*root)->data->service_name) < 0) {
        Binary_insert(&((*root)->left), data);
    } else {
        Binary_insert(&((*root)->right), data);
    }
    return *root;
}

Service* Binary_search(BinaryNode *root, const char *key) {
    if (root == NULL)
        return NULL;
    int cmp = strcmp(key, root->data->service_name);
    if (cmp == 0)
        return root->data;
    else if (cmp < 0)
        return Binary_search(root->left, key);
    else
        return Binary_search(root->right, key);
}

void Binary_free(BinaryNode *root) {
    if (root) {
        Binary_free(root->left);
        Binary_free(root->right);
        free(root);
    }
}
