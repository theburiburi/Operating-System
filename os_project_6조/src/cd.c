#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "TreeNode.h"
#include "findNode.h"

void cd(TreeNode* root, TreeNode** current, TreeNode** previous, const char* path) {
    if (!root || !current || !path) return;

    if (strcmp(path, "") == 0 || strcmp(path, "~") == 0) {
        const char* username = getenv("USER");
        if (!username) username = "user";

        char homePath[256];
        snprintf(homePath, sizeof(homePath), "/home/%s", username);

        TreeNode* home = findNodeByPath(root, homePath);
        if (home && home->isDirectory) {
            *previous = *current;
            *current = home;
        } else {
            printf("cd: 홈 디렉토리를 찾을 수 없습니다: %s\n", homePath);
        }
        return;
    }

    if (strcmp(path, "-") == 0) {
        if (*previous) {
            TreeNode* temp = *current;
            *current = *previous;
            *previous = temp;
        } else {
            printf("cd: 이전 디렉토리가 없습니다.\n");
        }
        return;
    }

    if (strcmp(path, "..") == 0) {
        if ((*current)->parent) {
            *previous = *current;
            *current = (*current)->parent;
        }
        return;
    }

    if (strcmp(path, ".") == 0) {
        return;
    }

    TreeNode* target = (path[0] == '/')
        ? findNodeByPath(root, path)
        : findNodeByRpath(*current, path);

    if (target && target->isDirectory) {
        *previous = *current;
        *current = target;
    } else {
        printf("cd: 디렉토리를 찾을 수 없습니다: %s\n", path);
    }
}

