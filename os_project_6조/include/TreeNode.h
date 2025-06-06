#ifndef TREENODE_H
#define TREENODE_H

#include <time.h>
#include <stddef.h>

typedef struct TreeNode {
    char name[256];              // 파일/디렉토리 이름
    int isDirectory;             // 디렉토리 여부 (1: 디렉토리, 0: 파일)
    size_t size;                 // 파일 사이즈 (단순 구현용)
    int links;                   // 하드링크 수
    char owner[256];            // 소유자 이름
    int ownerID;                // 소유자 userID
    char group[256];            // 그룹 이름 (선택적으로 확장 가능)
    int permissions;            // 권한 (10진수로 저장: 예 493 = 0755)
    time_t modifyTime;
    time_t changeTime;
    time_t accessTime;
    char* content;              // 파일 내용 (단순 문자열)
    struct TreeNode* parent;    // 부모 노드
    struct TreeNode* child;     // 첫 번째 자식
    struct TreeNode* sibling;   // 다음 형제 노드
} TreeNode;

TreeNode* createNode(const char* name, int isDir, int permissions);
void addChild(TreeNode* parent, TreeNode* child);

#endif

