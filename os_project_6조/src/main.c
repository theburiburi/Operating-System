#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TreeNode.h"
#include "addUser.h"
#include "pwd.h"
#include "alias.h"
#include "handleCommand.h"

int main() {
    TreeNode* root = createNode("", 1, 0755);
    const char* username = getenv("USER");
    if (!username) username = "user";

    User currentUser = addUser(root, username);
    TreeNode* current = currentUser.homeDir;
    TreeNode* previous = NULL;

    // 홈 디렉토리에 예시 파일 추가
    TreeNode* file1 = createNode("file1", 0, 0644);
    strncpy(file1->owner, currentUser.name, sizeof(file1->owner));
    file1->ownerID = currentUser.id;
    addChild(current, file1);

    char command[256];
    printf("Mini OS 시작 - 사용자 %s\n", currentUser.name);
    printf("사용 가능한 명령어: cat, mkdir, rmdir, clear, cd, pwd, chmod, adduser, ls, mv, alias, unalias, find, cp, exit\n\n");

    while (1) {
        printf("%s@mini_os:", currentUser.name);
        pwd(current);
        printf(" $ ");

        if (!fgets(command, sizeof(command), stdin))
            break;
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            break;

        } else if (strncmp(command, "alias", 5) == 0) {
            Alias(command);

        } else if (strncmp(command, "unalias", 7) == 0) {
            UnAlias(command + 8);

        } else {
            // alias 문자열 치환
            call_alias(command, root, &current, &previous);
            // 명령어 처리 위임
            handleCommand(root, &current, &previous, command);
        }
    }

    return 0;
}

