#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "TreeNode.h"
#include "findNode.h"

// 쓰레드에 전달할 인자 구조체 (파일 하나당 하나씩)
typedef struct {
    TreeNode* root;
    TreeNode* currentDir;
    int permBits;
    int mode;
    char target[256];
} ChmodThreadArgs;

// 트리 구조 변경 시 동시 접근 보호용 뮤텍스
static pthread_mutex_t tree_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// 8진수 표기를 10진수 권한 비트로 변환
static int oct_to_dec(int oct) {
    int dec = 0, base = 1;
    while (oct > 0) {
        int rem = oct % 10;
        dec += rem * base;
        base *= 8;
        oct /= 10;
    }
    return dec;
}

// 개별 파일에 대해 권한 변경을 수행할 쓰레드 함수
void* chmod_worker(void* arg) {
    ChmodThreadArgs* a = (ChmodThreadArgs*)arg;
    pthread_mutex_lock(&tree_mutex);
    TreeNode* node = (a->target[0]=='/') ? findNodeByPath(a->root, a->target)
                                         : findNodeByRpath(a->currentDir, a->target);
    if (node) node->permissions = a->permBits;
    pthread_mutex_unlock(&tree_mutex);

    pthread_mutex_lock(&print_mutex);
    if (node) {
        printf("chmod: '%s'의 권한이 %03d로 변경되었습니다.\n", a->target, a->mode);
    } else {
        printf("chmod: '%s' 파일을 찾을 수 없습니다.\n", a->target);
    }
    pthread_mutex_unlock(&print_mutex);

    free(a);
    return NULL;
}

// chmod 명령을 멀티스레딩으로 처리 (다중 파일 지원)
void chmod(TreeNode* root, TreeNode* currentDir, const char* args) {
    // 인자를 복사하여 토큰화
    char buf[512];
    strncpy(buf, args, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    // 첫 토큰: 모드
    char* saveptr = NULL;
    char* modeStr = strtok_r(buf, " ", &saveptr);
    if (!modeStr) {
        printf("사용법: chmod <권한(000~777)> <파일명> [파일명 ...]\n");
        return;
    }
    int mode;
    if (sscanf(modeStr, "%d", &mode) != 1) {
        printf("chmod: 권한 숫자를 올바르게 입력하세요.\n");
        return;
    }
    int d1 = mode/100, d2 = (mode/10)%10, d3 = mode%10;
    if (d1>7||d2>7||d3>7) {
        printf("chmod: 각 자릿수는 0~7 사이여야 합니다.\n");
        return;
    }
    int permBits = oct_to_dec(mode);

    // 토큰마다 쓰레드 생성
    pthread_t tids[64]; // 최대 64개 파일 지원
    int thread_count = 0;
    char* token;
    while ((token = strtok_r(NULL, " ", &saveptr))) {
        ChmodThreadArgs* a = malloc(sizeof(*a));
        if (!a) continue;
        a->root = root;
        a->currentDir = currentDir;
        a->permBits = permBits;
        a->mode = mode;
        strncpy(a->target, token, sizeof(a->target)-1);
        a->target[sizeof(a->target)-1] = '\0';

        if (pthread_create(&tids[thread_count], NULL, chmod_worker, a) == 0) {
            thread_count++;
        } else {
            free(a);
        }
        if (thread_count >= 64) break;
    }
    if (thread_count == 0) {
        printf("사용법: chmod <권한(000~777)> <파일명> [파일명 ...]\n");
        return;
    }

    // 모든 쓰레드 종료 대기
    for (int i = 0; i < thread_count; i++) {
        pthread_join(tids[i], NULL);
    }
}

