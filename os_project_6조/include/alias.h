#ifndef ALIAS_H
#define ALIAS_H
#include "TreeNode.h"

void Alias(const char* input);           // alias 등록
void UnAlias(const char* name);          // alias 삭제
int call_alias(char* command, TreeNode* root, TreeNode** current, TreeNode** previous);  // 명령어 치환 및 실행

#endif

