#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alias.h"
#include "TreeNode.h"

#define MAX_ALIAS 100

typedef struct {
    char name[50];     // 별칭 (예: ll)
    char command[256]; // 실제 명령어 (예: ls -l)
} AliasMap;

static AliasMap aliasList[MAX_ALIAS];
static int aliasCount = 0;

void Alias(const char* input) {
    char name[50], command[256];

    if (sscanf(input, "alias %49[^=]=%255[^\n]", name, command) != 2) {
        printf("사용법: alias 이름=\"명령어\"\n");
        return;
    }

    // 큰따옴표 제거
    size_t len = strlen(command);
    if (command[0] == '"' && command[len - 1] == '"') {
        command[len - 1] = '\0';
        memmove(command, command + 1, len - 1);
    }

    // 기존 alias 덮어쓰기
    for (int i = 0; i < aliasCount; ++i) {
        if (strcmp(aliasList[i].name, name) == 0) {
            strncpy(aliasList[i].command, command, sizeof(aliasList[i].command));
            return;
        }
    }

    // 새 alias 추가
    if (aliasCount < MAX_ALIAS) {
        strncpy(aliasList[aliasCount].name, name, sizeof(aliasList[aliasCount].name));
        strncpy(aliasList[aliasCount].command, command, sizeof(aliasList[aliasCount].command));
        aliasCount++;
    } else {
        printf("alias 개수가 너무 많습니다.\n");
    }
}

void UnAlias(const char* name) {
    for (int i = 0; i < aliasCount; ++i) {
        if (strcmp(aliasList[i].name, name) == 0) {
            for (int j = i; j < aliasCount - 1; ++j) {
                aliasList[j] = aliasList[j + 1];
            }
            aliasCount--;
            return;
        }
    }
    printf("alias '%s'를 찾을 수 없습니다.\n", name);
}

// 입력 명령어가 alias이면 command를 실제 명령어로 바꿔줌
int call_alias(char* command, TreeNode* root, TreeNode** current, TreeNode** previous) {

    (void)root;
    (void)current;
    (void)previous;

    for (int i = 0; i < aliasCount; ++i) {
        size_t len = strlen(aliasList[i].name);
        if (strncmp(command, aliasList[i].name, len) == 0 &&
            (command[len] == '\0' || command[len] == ' ')) {

            // 치환 명령어 + 인자 이어붙이기
            char replaced[256];
            snprintf(replaced, sizeof(replaced), "%s%s",
                     aliasList[i].command,
                     command + len); // 뒤에 붙은 인자들 그대로

            strncpy(command, replaced, 255);
            command[255] = '\0';
            return 1;  // alias로 치환 완료
        }
    }
    return 0;  // alias 아님
}

