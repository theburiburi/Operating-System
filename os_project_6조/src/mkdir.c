#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "TreeNode.h"
#include "mkdir.h"
#include "findNode.h"

typedef struct {
    TreeNode* parent;
    char name[256];
    int permission;
} ThreadArg;

typedef struct NodeList {
    TreeNode* node;
    struct NodeList* next;
} NodeList;

static pthread_mutex_t tree_mutex = PTHREAD_MUTEX_INITIALIZER;

NodeList* add_to_list(NodeList* head, TreeNode* node) {
    NodeList* new_node = malloc(sizeof(NodeList));
    new_node->node = node;
    new_node->next = head;
    return new_node;
}

NodeList* reverse_list(NodeList* head) {
    NodeList* prev = NULL;
    NodeList* curr = head;
    NodeList* next = NULL;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

void free_list(NodeList* head) {
    NodeList* temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int is_duplicate(TreeNode* current, const char* name) {
    TreeNode* child = current->child;
    while (child) {
        if (strcmp(child->name, name) == 0) return 1;
        child = child->sibling;
    }
    return 0;
}

int parse_mode_str(const char* mode_str) {
    if (strlen(mode_str) != 3) return -1;
    int perm = 0;
    for (int i = 0; i < 3; ++i) {
        if (mode_str[i] < '0' || mode_str[i] > '7') return -1;
        perm = perm * 8 + (mode_str[i] - '0');
    }
    return perm;
}

void format_permission(char* out, int perm, int isDir) {
    out[0] = isDir ? 'd' : '-';
    const char rwx[] = {'r','w','x'};
    for (int i = 0; i < 9; ++i) {
        out[i+1] = (perm & (1 << (8 - i))) ? rwx[i % 3] : '-';
    }
    out[10] = '\0';
}

void print_detailed_info(TreeNode* node) {
    char timebuf[64];
    struct tm* tm_info = localtime(&node->modifyTime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);

    char perm_str[11];
    format_permission(perm_str, node->permissions, node->isDirectory);

    printf("%s %d %-8s %-8s %5lu %s %s\n",
           perm_str,
           node->links,
           node->owner[0] ? node->owner : "user",
           node->group,
           node->size ? node->size : 4096,
           timebuf,
           node->name);
}

void* mkdir_thread(void* arg) {
    ThreadArg* t = (ThreadArg*)arg;

    pthread_mutex_lock(&tree_mutex);

    if (is_duplicate(t->parent, t->name)) {
        pthread_mutex_unlock(&tree_mutex);
        pthread_exit(NULL);
    }

    TreeNode* newDir = createNode(t->name, 1, t->permission);
    addChild(t->parent, newDir);
    newDir->links = 2;
    t->parent->links++;

    pthread_mutex_unlock(&tree_mutex);

    pthread_exit(newDir);
}

TreeNode* traverse_path(TreeNode* root, TreeNode* current, const char* path, int create, int permission, NodeList** outList) {
    char path_copy[1024];
    strncpy(path_copy, path, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';

    TreeNode* node = (path[0] == '/') ? root : current;
    char* token = strtok(path_copy, "/");

    while (token) {
        TreeNode* child = node->child;
        TreeNode* found = NULL;
        while (child) {
            if (strcmp(child->name, token) == 0 && child->isDirectory) {
                found = child;
                break;
            }
            child = child->sibling;
        }

        if (!found) {
            if (!create) return NULL;

            pthread_mutex_lock(&tree_mutex);
            TreeNode* newDir = createNode(token, 1, permission);
            addChild(node, newDir);
            newDir->links = 2;
            node->links++;
            pthread_mutex_unlock(&tree_mutex);

            *outList = add_to_list(*outList, newDir);
            node = newDir;
        } else {
            node = found;
        }

        token = strtok(NULL, "/");
    }

    return node;
}

void mkdir_multi(TreeNode* current, const char* input) {
    int make_parents = 0;
    int custom_permission = 0755;

    char* temp = strdup(input);
    char* tokens[20];
    int token_count = 0;

    char* token = strtok(temp, " ");
    while (token && token_count < 20) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }

    int start_idx = 0;
    if (token_count >= 1 && strcmp(tokens[0], "-p") == 0) {
        make_parents = 1;
        start_idx = 1;
    } else if (token_count >= 3 && strcmp(tokens[0], "-m") == 0) {
        int perm = parse_mode_str(tokens[1]);
        if (perm < 0) {
            printf("잘못된 권한 형식입니다. (예: 755)\n");
            free(temp);
            return;
        }
        custom_permission = perm;
        start_idx = 2;
    }

    if (start_idx >= token_count) {
        printf("경로를 입력하세요.\n");
        free(temp);
        return;
    }

    NodeList* created = NULL;

    for (int i = start_idx; i < token_count; ++i) {
        const char* path_arg = tokens[i];
        TreeNode* parent = NULL;
        char path_copy[1024];
        strncpy(path_copy, path_arg, sizeof(path_copy));
        path_copy[sizeof(path_copy) - 1] = '\0';

        char* last_slash = strrchr(path_copy, '/');
        char* dir_name = NULL;

        if (last_slash) {
            if (last_slash == path_copy) {
                parent = traverse_path(current, current, "/", make_parents, custom_permission, &created);
                dir_name = last_slash + 1;
            } else {
                *last_slash = '\0';
                parent = traverse_path(current, current, path_copy, make_parents, custom_permission, &created);
                dir_name = last_slash + 1;
            }
        } else {
            parent = current;
            dir_name = path_copy;
        }

        if (!parent) {
            printf("mkdir: '%s' 상위 디렉토리가 존재하지 않습니다.\n", path_arg);
            continue;
        }

        if (is_duplicate(parent, dir_name)) {
            printf("mkdir: '%s' 이미 존재합니다.\n", path_arg);
            continue;
        }

        ThreadArg* arg = malloc(sizeof(ThreadArg));
        strncpy(arg->name, dir_name, sizeof(arg->name));
        arg->name[sizeof(arg->name) - 1] = '\0';
        arg->parent = parent;
        arg->permission = custom_permission;

        pthread_t tid;
        void* thread_result = NULL;
        pthread_create(&tid, NULL, mkdir_thread, arg);
        pthread_join(tid, &thread_result);

        if (thread_result != NULL) {
            created = add_to_list(created, (TreeNode*)thread_result);
        }

        free(arg);
    }

    created = reverse_list(created);
    NodeList* ptr = created;
    while (ptr) {
        print_detailed_info(ptr->node);
        ptr = ptr->next;
    }

    free_list(created);
    free(temp);
}

