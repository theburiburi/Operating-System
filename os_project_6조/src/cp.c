#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "TreeNode.h"
#include "findNode.h"
#include "cp.h"

typedef struct {
    TreeNode* srcNode;
    TreeNode* destNode;
    int recursive;
    pthread_mutex_t* mutex;
} CpThreadArgs;

TreeNode* deepCopyNode(TreeNode* src, int recursive) {
    if (!src) return NULL;

    TreeNode* copy = createNode(src->name, src->isDirectory, src->permissions);
    copy->size = src->size;
    if (src->content) {
        copy->content = strdup(src->content);
    }

    if (recursive && src->isDirectory) {
        TreeNode* child = src->child;
        while (child) {
            TreeNode* childCopy = deepCopyNode(child, recursive);
            addChild(copy, childCopy);
            child = child->sibling;
        }
    }

    return copy;
}

void* copy_worker(void* arg) {
    CpThreadArgs* args = (CpThreadArgs*)arg;
    TreeNode* copiedNode = deepCopyNode(args->srcNode, args->recursive);

    pthread_mutex_lock(args->mutex);
    addChild(args->destNode, copiedNode);
    pthread_mutex_unlock(args->mutex);

    printf("Copied %s to %s\n", args->srcNode->name, args->destNode->name);
    free(arg);
    pthread_exit(NULL);
}

void cp(TreeNode* current, const char* command) {
    char cmd[512];
    int recursive = 0;

    strncpy(cmd, command, sizeof(cmd));
    char* token = strtok(cmd, " ");
    token = strtok(NULL, " ");  // skip 'cp'

    if (!token) {
        printf("Usage: cp [-r] source1 [source2 ...] dest\n");
        return;
    }

    if (strcmp(token, "-r") == 0) {
        recursive = 1;
        token = strtok(NULL, " ");
    }

    // 수집 배열
    char* srcPaths[64];
    int srcCount = 0;
    char* destPath = NULL;

    // 모든 토큰 순회
    while (token) {
        char* next = strtok(NULL, " ");
        if (next) {
            srcPaths[srcCount++] = token;
        } else {
            destPath = token;  // 마지막 토큰
        }
        token = next;
    }

    if (srcCount == 0 || destPath == NULL) {
        printf("Usage: cp [-r] source1 [source2 ...] dest\n");
        return;
    }

    TreeNode* destNode = (destPath[0] == '/') ? findNodeByPath(current, destPath) : findNodeByRpath(current, destPath);
    if (!destNode || !destNode->isDirectory) {
        printf("cp: 대상 디렉터리 '%s'가 존재하지 않습니다.\n", destPath);
        return;
    }

    pthread_t threads[64];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < srcCount; i++) {
        TreeNode* srcNode = (srcPaths[i][0] == '/') ? findNodeByPath(current, srcPaths[i]) : findNodeByRpath(current, srcPaths[i]);
        if (!srcNode) {
            printf("cp: '%s' 경로를 찾을 수 없습니다.\n", srcPaths[i]);
            continue;
        }

        CpThreadArgs* args = malloc(sizeof(CpThreadArgs));
        args->srcNode = srcNode;
        args->destNode = destNode;
        args->recursive = recursive;
        args->mutex = &mutex;

        if (pthread_create(&threads[i], NULL, copy_worker, args) != 0) {
            printf("cp: 스레드 생성 실패 (%s)\n", srcPaths[i]);
            free(args);
        }
    }

    for (int i = 0; i < srcCount; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
}
