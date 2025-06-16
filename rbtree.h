#include "service.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    Service *service;         // данные услуги
    Color color;              // цвет узла
    struct RBNode *left;      // левый потомок
    struct RBNode *right;     // правый потомок
    struct RBNode *parent;    // родительский узел
} RBNode;

// Создание нового узла дерева (новый узел всегда RED)
RBNode* create_node_rbtree(Service *service) {
    RBNode *node = (RBNode*)malloc(sizeof(RBNode));
    if (!node)
        exit(1);
    node->service = service;
    node->color = RED;
    node->left = node->right = node->parent = NULL;
    return node;
}

// Левый поворот: текущий узел становится левым потомком своего правого потомка.
void left_rotate(RBNode **root, RBNode *nodeCurrent) {
    RBNode *nodePivot = nodeCurrent->right;     // опорный узел — правый потомок
    nodeCurrent->right = nodePivot->left;         // переносим левое поддерево опорного узла в правое поддерево
    if (nodePivot->left != NULL)
        nodePivot->left->parent = nodeCurrent;
    nodePivot->parent = nodeCurrent->parent;
    if (nodeCurrent->parent == NULL)
        *root = nodePivot;
    else if (nodeCurrent == nodeCurrent->parent->left)
        nodeCurrent->parent->left = nodePivot;
    else
        nodeCurrent->parent->right = nodePivot;
    nodePivot->left = nodeCurrent;
    nodeCurrent->parent = nodePivot;
}

// Правый поворот: текущий узел становится правым потомком своего левого потомка.
void right_rotate(RBNode **root, RBNode *nodeCurrent) {
    RBNode *nodePivot = nodeCurrent->left;        // опорный узел — левый потомок
    nodeCurrent->left = nodePivot->right;           // переносим правое поддерево опорного узла в левое поддерево
    if (nodePivot->right != NULL)
        nodePivot->right->parent = nodeCurrent;
    nodePivot->parent = nodeCurrent->parent;
    if (nodeCurrent->parent == NULL)
        *root = nodePivot;
    else if (nodeCurrent == nodeCurrent->parent->right)
        nodeCurrent->parent->right = nodePivot;
    else
        nodeCurrent->parent->left = nodePivot;
    nodePivot->right = nodeCurrent;
    nodeCurrent->parent = nodePivot;
}

// Корректировка дерева после вставки нового узла для сохранения свойств красно-чёрного дерева.
void fix_insert(RBNode **root, RBNode *nodeInserted) {
    while (nodeInserted != *root && nodeInserted->parent->color == RED) {
        RBNode *grandParent = nodeInserted->parent->parent;
        if (nodeInserted->parent == grandParent->left) {
            RBNode *uncle = grandParent->right;
            if (uncle && uncle->color == RED) {
                // Случай 1: перекраска
                nodeInserted->parent->color = BLACK;
                uncle->color = BLACK;
                grandParent->color = RED;
                nodeInserted = grandParent;
            } else {
                if (nodeInserted == nodeInserted->parent->right) {
                    // Случай 2: левый поворот, если узел находится правее родителя
                    nodeInserted = nodeInserted->parent;
                    left_rotate(root, nodeInserted);
                }
                // Случай 3: правый поворот
                nodeInserted->parent->color = BLACK;
                grandParent->color = RED;
                right_rotate(root, grandParent);
            }
        } else {
            RBNode *uncle = grandParent->left;
            if (uncle && uncle->color == RED) {
                // Случай 1: перекраска
                nodeInserted->parent->color = BLACK;
                uncle->color = BLACK;
                grandParent->color = RED;
                nodeInserted = grandParent;
            } else {
                if (nodeInserted == nodeInserted->parent->left) {
                    // Случай 2: правый поворот, если узел находится левее родителя
                    nodeInserted = nodeInserted->parent;
                    right_rotate(root, nodeInserted);
                }
                // Случай 3: левый поворот
                nodeInserted->parent->color = BLACK;
                grandParent->color = RED;
                left_rotate(root, grandParent);
            }
        }
    }
    (*root)->color = BLACK;
}

// Вставка элемента в красно-чёрное дерево.
void rb_insert(RBNode **root, Service *service) {
    RBNode *nodeToInsert = create_node_rbtree(service);
    RBNode *parentNode = NULL;
    RBNode *currentNode = *root;
    while (currentNode != NULL) {
        parentNode = currentNode;
        if (strcmp(nodeToInsert->service->service_name, currentNode->service->service_name) < 0)
            currentNode = currentNode->left;
        else
            currentNode = currentNode->right;
    }
    nodeToInsert->parent = parentNode;
    if (parentNode == NULL)
        *root = nodeToInsert;
    else if (strcmp(nodeToInsert->service->service_name, parentNode->service->service_name) < 0)
        parentNode->left = nodeToInsert;
    else
        parentNode->right = nodeToInsert;
    
    fix_insert(root, nodeToInsert);
}

// Поиск элемента в красно-чёрном дереве по ключу.
Service* rb_search(RBNode *root, const char *searchKey) {
    while (root != NULL) {
        int cmpResult = strcmp(searchKey, root->service->service_name);
        if (cmpResult == 0)
            return root->service;
        else if (cmpResult < 0)
            root = root->left;
        else
            root = root->right;
    }
    return NULL;
}

// Рекурсивное освобождение памяти дерева.
void rb_free(RBNode *root) {
    if (root) {
        rb_free(root->left);
        rb_free(root->right);
        free(root);
    }
}
