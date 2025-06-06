#ifndef ADDUSER_H
#define ADDUSER_H

#include "TreeNode.h"

typedef struct User {
    int id;
    char name[256];
    TreeNode* homeDir;
} User;

User addUser(TreeNode* root, const char* username);

#endif

