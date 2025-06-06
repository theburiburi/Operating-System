#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "TreeNode.h"

TreeNode* createNode(const char* name, int isDir, int permissions) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        perror("메모리 할당 실패");
        exit(EXIT_FAILURE);
    }

    strncpy(node->name, name, sizeof(node->name));
    node->name[sizeof(node->name) - 1] = '\0';

    node->isDirectory = isDir;
    node->size = 0;
    node->links = 1;
    node->owner[0] = '\0';       // 추후 사용자 정보에서 설정
    node->ownerID = -1;          // 기본값: 무소유 상태
    strcpy(node->group, "user"); // 기본 그룹 설정 (옵션)
    node->permissions = permissions;

    node->modifyTime = time(NULL);
    node->changeTime = node->modifyTime;
    node->accessTime = node->modifyTime;

    node->content = NULL;
    node->parent = NULL;
    node->child = NULL;
    node->sibling = NULL;

    return node;
}

void addChild(TreeNode* parent, TreeNode* child) {
    if (!parent || !child) return;
    child->parent = parent;

    if (!parent->child) {
        parent->child = child;
    } else {
        TreeNode* sibling = parent->child;
        while (sibling->sibling) {
            sibling = sibling->sibling;
        }
        sibling->sibling = child;
    }
}

