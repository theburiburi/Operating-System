#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TreeNode.h"
#include "findNode.h"

TreeNode* findNodeByPath(TreeNode* root, const char* path) {
    if (!root || path[0] != '/') return NULL;

    char pathCopy[1024];
    strncpy(pathCopy, path, sizeof(pathCopy));
    pathCopy[sizeof(pathCopy) - 1] = '\0';

    TreeNode* current = root;

    char* token = strtok(pathCopy, "/");
    while (token != NULL && current != NULL) {
        TreeNode* child = current->child;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                current = child;
                break;
            }
            child = child->sibling;
        }
        if (child == NULL) return NULL;
        token = strtok(NULL, "/");
    }

    return current;
}

TreeNode* findNodeByRpath(TreeNode* current, const char* path) {
    if (!current || path[0] == '/') return NULL;

    char pathCopy[1024];
    strncpy(pathCopy, path, sizeof(pathCopy));
    pathCopy[sizeof(pathCopy) - 1] = '\0';

    TreeNode* node = current;

    char* token = strtok(pathCopy, "/");
    while (token != NULL && node != NULL) {
        if (strcmp(token, "..") == 0) {
            node = node->parent;
        } else if (strcmp(token, ".") == 0) {
            // stay in current
        } else {
            TreeNode* child = node->child;
            while (child != NULL) {
                if (strcmp(child->name, token) == 0) {
                    node = child;
                    break;
                }
                child = child->sibling;
            }
            if (child == NULL) return NULL;
        }
        token = strtok(NULL, "/");
    }

    return node;
}

