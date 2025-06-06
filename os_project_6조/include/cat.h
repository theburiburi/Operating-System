#ifndef CAT_H
#define CAT_H



#include "TreeNode.h"

void cat_read(TreeNode* currentDirectory, const char* filename);
void cat_write(TreeNode* currentDirectory, const char* filename);
void handle_cat(const char* arg, TreeNode* currentDirectory);
#endif
