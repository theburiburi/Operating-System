#include <stdio.h>
#include <string.h>
#include "pwd.h"
#include "TreeNode.h"  // TreeNode 구조 정의 포함

void pwd(TreeNode* current) {
    if (current == NULL) {
        printf("/");
        return;
    }

    TreeNode* stack[100];  // 최대 깊이 100
    int depth = 0;

    TreeNode* temp = current;
    while (temp != NULL && temp->parent != NULL) {
        stack[depth++] = temp;
        temp = temp->parent;
    }

    printf("/");  // 루트 디렉토리 시작

    for (int i = depth - 1; i >= 0; i--) {
        printf("%s", stack[i]->name);
        if (i > 0) printf("/");
    }
}

