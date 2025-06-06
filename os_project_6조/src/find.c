#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "find.h"

// 내부 재귀 검색 함수
static int matchPattern(const char* name, const char* pattern) {
    if (*pattern == '\0') return *name == '\0';

    if (*pattern == '*') {
        return matchPattern(name, pattern + 1) || 
               (*name && matchPattern(name + 1, pattern));
    } else if (*pattern == '?') {
        return *name && matchPattern(name + 1, pattern + 1);
    } else {
        return *name == *pattern && matchPattern(name + 1, pattern + 1);
    }
}

static void recursiveFind(TreeNode* node, const char* targetPattern, char* path) {
    if (!node) return;

    char newPath[1024];
    snprintf(newPath, sizeof(newPath), "%s/%s", path, node->name);

    if (matchPattern(node->name, targetPattern)) {
        printf("%s\n", newPath);
    }

    recursiveFind(node->child, targetPattern, newPath);
    recursiveFind(node->sibling, targetPattern, path);
}

// 외부 호출용: handleCommand에서 호출
void handle_find(const char* command, TreeNode* current) {
    const char* pattern = command + 5;
    while (*pattern == ' ') pattern++;

    if (*pattern == '\0') {
        printf("사용법: find <이름 또는 패턴>\n");
        return;
    }

    char path[1024] = "";
    recursiveFind(current, pattern, path);
}

