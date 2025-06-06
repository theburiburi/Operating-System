#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TreeNode.h"
#include "addUser.h"
#include "findNode.h"

User addUser(TreeNode* root, const char* username) {
    static int nextUserID = 1000;

    TreeNode* home = findNodeByPath(root, "/home");
    if (!home) {
        home = createNode("home", 1, 0755);
        addChild(root, home);
    }

    TreeNode* userDir = createNode(username, 1, 0755);
    addChild(home, userDir);
    strncpy(userDir->owner, username, sizeof(userDir->owner));
    userDir->ownerID = nextUserID;

    User user;
    user.id = nextUserID++;
    strncpy(user.name, username, sizeof(user.name));
    user.name[sizeof(user.name) - 1] = '\0';
    user.homeDir = userDir;

    return user;
}

