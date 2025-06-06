#include <stdio.h>
#include <string.h>
#include "handleCommand.h"
#include "cd.h"
#include "pwd.h"
#include "chmod.h"
#include "ls.h"
#include "cat.h"
#include "mkdir.h"
#include "rmdir.h"
#include "mv.h"
#include "clear.h"
#include "find.h"
#include "cp.h"

void handleCommand(TreeNode* root, TreeNode** current, TreeNode** previous, const char* command) {
    if (strcmp(command, "pwd") == 0) {
        pwd(*current);
        printf("\n");

    } else if (strncmp(command, "cd", 2) == 0) {
        const char* arg = command + 2;
        while (*arg == ' ') arg++;
        cd(root, current, previous, (*arg == '\0') ? "~" : arg);

    } else if (strncmp(command, "ls", 2) == 0) {
        process_ls(command, *current);

    } else if (strcmp(command, "clear") == 0) {
        execute_clear();

    } else if (strncmp(command, "cat", 3) == 0) {
        handle_cat(command + 3, *current);

    } else if (strncmp(command, "mkdir ", 6) == 0) {
        mkdir_multi(*current, command + 6);

    } else if (strncmp(command, "rmdir ", 6) == 0) {
        rmdir_multi(*current, command + 6);

    } else if (strncmp(command, "mv ", 3) == 0) {
        char from[128], to[128];
        if (sscanf(command + 3, "%127s %127s", from, to) == 2) {
            mv(*current, from, to);
        }

    } else if (strncmp(command, "chmod ", 6) == 0) {
        chmod(root, *current, command + 6);

    } else if (strncmp(command, "find ", 5) == 0) {
        handle_find(command, *current);

    } else if (strncmp(command, "cp", 2) == 0) {
        cp(*current, command);

    } else {
        printf("알 수 없는 명령어입니다: %s\n", command);
    }
}

