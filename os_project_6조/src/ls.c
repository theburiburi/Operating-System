#include <stdio.h>
#include <string.h>
#include <time.h>
#include "TreeNode.h"
#include "findNode.h"

void printPermissions(int permissions) {
    printf((permissions & 0400) ? "r" : "-");
    printf((permissions & 0200) ? "w" : "-");
    printf((permissions & 0100) ? "x" : "-");
    printf((permissions & 0040) ? "r" : "-");
    printf((permissions & 0020) ? "w" : "-");
    printf((permissions & 0010) ? "x" : "-");
    printf((permissions & 0004) ? "r" : "-");
    printf((permissions & 0002) ? "w" : "-");
    printf((permissions & 0001) ? "x" : "-");
}

void ls(TreeNode* target, int showall, int showlong) {
    if(!target || !target->isDirectory) return;

    TreeNode* child = target->child;
    while (child != NULL) {
        if (!showall && child->name[0] == '.') {
            child = child->sibling;
            continue;
        }

        if (showlong) {
            printf("%c", child->isDirectory ? 'd' : '-');
            printPermissions(child->permissions);
            printf(" %d %s %s %ld ", child->links, child->owner, child->group, child->size);

            printf("%.24s ", ctime(&child->modifyTime));
        }

        printf("%s\n", child->name);
        child = child->sibling;
    }
}

void process_ls(const char* command, TreeNode* current) {
    int showall = 0, showlong = 0;
    const char* arg = command + 2;

    while (*arg == ' ') arg++;
    
    char path[1024] = "";
    if (*arg == '-') {
        arg++;
        while (*arg && *arg != ' ') {
            if (*arg == 'a') showall = 1;
            else if (*arg == 'l') showlong = 1;
            arg++;
        }
	while (*arg == ' ') arg++;
    }
    

    if (*arg) {
	strncpy(path, arg, sizeof(path));
	path[sizeof(path) - 1] = '\0';
    }


    TreeNode* target = current;
    if (strlen(path) > 0) {
        target = (path[0] == '/') ? findNodeByPath(current, path) : findNodeByRpath(current, path);
        if (!target) {
            printf("ls: '%s' 디렉터리를 찾을 수 없습니다.\n", path);
            return;
        }
    }

    ls(target, showall, showlong);
}

