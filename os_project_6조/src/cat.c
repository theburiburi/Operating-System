#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TreeNode.h"
#include "cat.h"
#include <time.h>
#include "findNode.h"

//기본값 추가
void initializeFileMetadata(TreeNode* node) {
    if (!node) return;
 
    const char* username = getenv("USER");
    if (!username) username = "user";


    strcpy(node->owner, username);     
    strcpy(node->group, "user");
 
    node->links = 1;

      time_t currentTime = time(NULL);
    node->modifyTime = currentTime;
    node->accessTime = currentTime;
    node->changeTime = currentTime;
}


void cat_read(TreeNode* currentDirectory, const char* filename) {
    TreeNode* target = findNodeByRpath(currentDirectory, filename);

    if (!target) {
        printf("cat: '%s': 그런 파일 없음\n", filename);
        return;
    }

    if (target->isDirectory) {
        printf("cat: '%s': 디렉토리입니다\n", filename);
        return;
    }

    if (target->content) {
        printf("%s \n", target->content);
    } else {
        printf("cat: '%s': 내용 없음\n", filename);
    }
}
void cat_write(TreeNode* currentDirectory, const char* filename) {
    TreeNode* fileNode = createNode(filename, 0, 0644); 
    printf("파일 내용 입력 (Ctrl+D로 종료):\n");
    
    initializeFileMetadata(fileNode);
    
    char buffer[1000];
    fileNode->content = malloc(10000);  // 동적 할당
    fileNode->content[0] = '\0';

    while (fgets(buffer, sizeof(buffer), stdin)) {
        strcat(fileNode->content, buffer);
    }

    fileNode->size = strlen(fileNode->content);

    addChild(currentDirectory, fileNode);
    printf("'%s' 파일이 가상 디렉토리에 생성되었습니다.\n", filename);
    clearerr(stdin);
}

void handle_cat(const char* arg, TreeNode* currentDirectory) {
    while (*arg == ' ') arg++;

    if (strncmp(arg, ">", 1) == 0) {
        arg++; while (*arg == ' ') arg++;
        if (*arg) cat_write(currentDirectory, arg);
        else printf("cat: 파일 이름이 필요합니다\n");
    } else if (*arg) {
        cat_read(currentDirectory, arg);
    } else {
        printf("사용법: cat [filename] 또는 cat > [filename]\n");
    }
}
