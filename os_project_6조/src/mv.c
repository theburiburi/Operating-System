#include <stdio.h>
#include <string.h>
#include "TreeNode.h"
#include "findNode.h"
#include "mv.h"

void removeChild(TreeNode* parent, TreeNode* child) {
    if (!parent || !child) return;

    TreeNode* prev = NULL;
    TreeNode* curr = parent->child;

    while (curr) {
        if (curr == child) {
            if (prev == NULL) {
                parent->child = curr->sibling;
            } else {
                prev->sibling = curr->sibling;
            }
            child->sibling = NULL;
            return;
        }
        prev = curr;
        curr = curr->sibling;
    }
}

void mv(TreeNode* current, const char* src, const char* dest) {
    if (!current || !src || !dest) return;

    TreeNode* srcNode = (src[0] == '/') ? findNodeByPath(current, src) : findNodeByRpath(current, src);
    if (!srcNode) {
        printf("mv: '%s' 경로를 찾을 수 없습니다.\n", src);
        return;
    }

    TreeNode* destNode = (dest[0] == '/') ? findNodeByPath(current, dest) : findNodeByRpath(current, dest);

    if (destNode && destNode->isDirectory) {
        removeChild(srcNode->parent, srcNode);
        addChild(destNode, srcNode);
        return;
    }

    char parentPath[1024];
    strncpy(parentPath, dest, sizeof(parentPath));
    parentPath[sizeof(parentPath) - 1] = '\0';

    char* lastSlash = strrchr(parentPath, '/');
    char newName[256];

    if (lastSlash) {
        *lastSlash = '\0';  
        strncpy(newName, lastSlash + 1, sizeof(newName));
        newName[sizeof(newName) - 1] = '\0';
    } else {
        strcpy(newName, dest);
        strcpy(parentPath, ".");
    }

    TreeNode* newParent = (parentPath[0] == '/') ? findNodeByPath(current, parentPath) : findNodeByRpath(current, parentPath);

    if (!newParent || !newParent->isDirectory) {
        printf("mv: 대상 디렉터리 '%s'가 존재하지 않습니다.\n", parentPath);
        return;
    }

    strncpy(srcNode->name, newName, sizeof(srcNode->name));
    srcNode->name[sizeof(srcNode->name) - 1] = '\0';

    removeChild(srcNode->parent, srcNode);
    addChild(newParent, srcNode);
}
