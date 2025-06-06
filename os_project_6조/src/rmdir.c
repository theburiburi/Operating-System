#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "TreeNode.h"
#include "rmdir.h"

typedef struct {
    TreeNode* current;
    char path[256];
    int is_p_option;
} RmdirArg;

static pthread_mutex_t tree_mutex = PTHREAD_MUTEX_INITIALIZER;

TreeNode* traversePath(TreeNode* start, const char* path) {
    if (!path || strlen(path) == 0) return start;

    TreeNode* current = start;
    char path_copy[256];
    strncpy(path_copy, path, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';

    char* token = strtok(path_copy, "/");
    while (token) {
        TreeNode* child = current->child;
        TreeNode* found = NULL;
        while (child) {
            if (strcmp(child->name, token) == 0 && child->isDirectory) {
                found = child;
                break;
            }
            child = child->sibling;
        }
        if (!found) return NULL;
        current = found;
        token = strtok(NULL, "/");
    }

    return current;
}

TreeNode* getParentFromPath(TreeNode* start, const char* path, char* outName) {
    char path_copy[256];
    strncpy(path_copy, path, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';

    char* last_slash = strrchr(path_copy, '/');
    if (!last_slash) {
        strcpy(outName, path_copy);
        return start;
    }

    *last_slash = '\0';
    strcpy(outName, last_slash + 1);
    return traversePath(start, path_copy);
}

void* rmdir_thread(void* arg) {
    RmdirArg* r = (RmdirArg*)arg;

    pthread_mutex_lock(&tree_mutex);

    if (r->is_p_option) {
        char path_copy[256];
        strncpy(path_copy, r->path, sizeof(path_copy));
        path_copy[sizeof(path_copy) - 1] = '\0';

        char* segments[32];
        int depth = 0;
        char* token = strtok(path_copy, "/");
        while (token) {
            segments[depth++] = token;
            token = strtok(NULL, "/");
        }

        for (int i = depth; i > 0; --i) {
            TreeNode* current = r->current;
            TreeNode* parent = current;
            for (int j = 0; j < i - 1; ++j) {
                TreeNode* child = parent->child;
                while (child && !(child->isDirectory && strcmp(child->name, segments[j]) == 0)) {
                    child = child->sibling;
                }
                if (!child) goto done;
                parent = child;
            }

            TreeNode* target = parent->child;
            TreeNode* prev = NULL;
            while (target && strcmp(target->name, segments[i - 1]) != 0) {
                prev = target;
                target = target->sibling;
            }

            if (!target || !target->isDirectory) goto done;
            if (target->child != NULL) goto done;
            if (!(target->permissions & 0100)) {
                printf("rmdir: '%s' \u00b7 실행 권한이 없습니다.\n", target->name);
                goto done;
            }

            if (prev)
                prev->sibling = target->sibling;
            else
                parent->child = target->sibling;

            printf("rmdir: '%s' 삭제 완료\n", target->name);
            free(target);
        }

    } else {
        char name[256];
        TreeNode* parent = getParentFromPath(r->current, r->path, name);
        if (!parent) {
            printf("rmdir: '%s' 경로를 찾을 수 없습니다.\n", r->path);
            goto done;
        }

        TreeNode* prev = NULL;
        TreeNode* child = parent->child;
        while (child) {
            if (strcmp(child->name, name) == 0) {
                if (!child->isDirectory) {
                    printf("rmdir: '%s'는 디렉토리가 아닙니다.\n", name);
                    goto done;
                }
                if (!(child->permissions & 0100)) {
                    printf("rmdir: '%s' \u00b7 실행 권한이 없습니다.\n", name);
                    goto done;
                }
                if (child->child != NULL) {
                    printf("rmdir: '%s'는 비어있지 않습니다.\n", name);
                    goto done;
                }

                if (prev)
                    prev->sibling = child->sibling;
                else
                    parent->child = child->sibling;

                printf("rmdir: '%s' 삭제 완료\n", name);
                free(child);
                goto done;
            }
            prev = child;
            child = child->sibling;
        }

        printf("rmdir: '%s' 디렉토리를 찾을 수 없습니다.\n", name);
    }

done:
    pthread_mutex_unlock(&tree_mutex);
    pthread_exit(NULL);
}

void rmdir_multi(TreeNode* current, const char* input) {
    char* tmp = strdup(input);
    char* tokens[16];
    int count = 0;

    char* token = strtok(tmp, " ");
    while (token && count < 16) {
        tokens[count++] = token;
        token = strtok(NULL, " ");
    }

    int is_p_option = 0;
    int start = 0;
    if (count > 1 && strcmp(tokens[0], "-p") == 0) {
        is_p_option = 1;
        start = 1;
    }

    pthread_t threads[10];
    RmdirArg* args[10];
    int thread_count = 0;

    for (int i = start; i < count && thread_count < 10; ++i) {
        args[thread_count] = malloc(sizeof(RmdirArg));
        args[thread_count]->current = current;
        args[thread_count]->is_p_option = is_p_option;
        strncpy(args[thread_count]->path, tokens[i], sizeof(args[thread_count]->path));
        args[thread_count]->path[sizeof(args[thread_count]->path) - 1] = '\0';

        pthread_create(&threads[thread_count], NULL, rmdir_thread, args[thread_count]);
        thread_count++;
    }

    for (int i = 0; i < thread_count; ++i) {
        pthread_join(threads[i], NULL);
        free(args[i]);
    }

    free(tmp);
}

