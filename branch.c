#include "commit.c"

// 初始化分支，将当前分支设置为 master
void initBranch() {
    system("touch .currentbranch"); // 创建 .currentbranch 文件
    FILE* f = fopen(".currentbranch", "w"); // 打开名为 .currentbranch 的文件，以写模式
    if(f == NULL) {
        printf("Erreur d'ouverture!");
        exit(1);
    }
    fputs("master", f); // 将字符串 "master" 写入文件
    fclose(f); // 关闭文件
}

// 判断分支是否存在
int branchExists(char* branch) {
    // 列出 .refs 目录下所有文件
    List* refs = listdir(".refs");
    // 在列表中查找分支名
    return searchList(refs, branch) != NULL;
}

// 创建分支
void createBranch(char* branch) {
    // 获取当前分支的哈希值
    char* hash = getRef("HEAD");
    // 在 .refs 目录下创建或更新分支引用
    createUpdateRef(branch, hash);
}

// 从名为 .currentbranch 的文件中获取当前所在的分支名称
char* getCurrentBranch() {
    FILE* f = fopen(".currentbranch", "r");
    char* buff = malloc(sizeof(char) * 100);
    fscanf(f, "%s", buff);
    return buff;
}

// 将哈希值转换成 Commit 文件路径
char *hashToPathCommit(char *hash) {
    char *buff = malloc(sizeof(char) * 100);
    sprintf(buff, "%s.c", hashToPath(hash));
    return buff;
}

// 打印指定分支的提交记录
void printBranch(char *branch) {
    char *commit_hash = getRef(branch); // 获取分支的最新提交的哈希值
    Commit *c = ftc(hashToPathCommit(commit_hash)); // 获取最新提交对应的 Commit 对象
    while (c != NULL) {
        if (commitGet(c, "message") != NULL) { // 如果提交记录中有 message 属性，则打印哈希值和 message
            printf("%s -> %s \n", commit_hash, commitGet(c, "message"));
        } else { // 如果没有 message 属性，则只打印哈希值
            printf("%s \n", commit_hash);
        }
        if (commitGet(c, "predecessor") != NULL) { // 如果有前置提交，则获取前置提交的哈希值，并继续循环
            commit_hash = commitGet(c, "predecessor");
            c = ftc(hashToPathCommit(commit_hash));
        } else { // 如果没有前置提交，则循环结束
            c = NULL;
        }
    }
}

// 返回指定分支的提交记录哈希列表
List * branchList(char * branch) {
    List * L = initList(); // 创建一个新的链表
    char * commit_hash = getRef(branch); // 获取分支的最新提交的哈希值
    Commit * c = ftc(hashToPathCommit(commit_hash)); // 从哈希值获取 Commit 对象

    while (c != NULL) {
        insertFirst(L, buildCell(commit_hash)); // 在链表头插入节点，存储当前哈希值
        if (commitGet(c, "predecessor") != NULL) {
            commit_hash = commitGet(c, "predecessor");
            c = ftc(hashToPathCommit(commit_hash)); // 获取前一个 Commit 对象
        } else {
            c = NULL; // 已到达分支起点，退出循环
        }
    }
    return L;
}

// 获取所有 commit
List* getAllCommits() {
    List* L = initList(); // 初始化列表
    List* content = listdir(".refs"); // 获取 .refs 目录下的文件列表
    for (Cell* ptr = *content; ptr != NULL; ptr = ptr->next) { // 遍历列表
        if (ptr->data[0] == '.') { // 跳过以 . 开头的文件（如 .add、.currentbranch）
            continue;
        }
        List* list = branchList(ptr->data); // 获取该分支的所有 commit
        Cell* cell = *list;
        while (cell != NULL) { // 遍历 commit 列表
            if (searchList(L, cell->data) == NULL) { // 如果 commit 不在列表中，则加入
                insertFirst(L, buildCell(cell->data));
            }
            cell = cell->next;
        }
    }
    return L;
}

// 通过 commit hash 值，将对应 commit 中的文件恢复到工作目录中
void restoreCommit(char* hash_commit) {
    // 获取 commit 对应文件路径
    char* commit_path = hashToPathCommit(hash_commit);
    // 通过 commit 文件路径获取对应的 commit 结构体
    Commit* c = ftc(commit_path);
    // 获取 commit 结构体中的 tree hash 值，再转换为 tree 文件路径
    char* tree_hash = hashToPath(commitGet(c, "tree"));
    char tmp[100];
    if(tree_hash[strlen(tree_hash) + 1] == '\0') {
        strncpy(tmp, tree_hash, strlen(tree_hash));
    }
    char* hash_final = strcat(tmp, ".t");
    printf("%s\n", hash_final);
    hash_final[strlen(hash_final)] = '\0';
    // 通过 tree 文件路径获取 WorkTree 结构体
    WorkTree* wt = ftwt(hash_final);
    // 将 WorkTree 中的文件恢复到工作目录中
    restoreWorkTree(wt, ".");
}

// 切换分支
void myGitCheckoutBranch(char* branch) {
    // 修改当前分支
    FILE* f = fopen(".currentbranch", "w");
    fprintf(f, "%s", branch);
    fclose(f);
    // 获取分支最新的commit hash
    char* hash_commit = getRef(branch);
    // 更新HEAD引用
    createUpdateRef("HEAD", hash_commit);

    // 恢复commit对应的work tree
    restoreCommit(hash_commit);
}

// 根据模式过滤链表
List* filterList(List* L, char* pattern) {
    List* filtered = initList(); // 初始化链表
    for (Cell* ptr = *L; ptr != NULL; ptr = ptr->next) { // 遍历原链表
        char* c = strdup(ptr->data); // 复制字符串
        c[strlen(pattern)] = '\0'; // 将模式后面的字符置为'\0'
        if (strcmp(c, pattern) == 0) { // 如果与模式匹配
            insertFirst(filtered, buildCell(ptr->data)); // 将其添加到结果链表中
        }
        free(c); // 释放内存
    }
    return filtered; // 返回结果链表
}

void myGitCheckoutCommit(char *pattern) {
    // 获取所有 commit 的列表
    List *L = getAllCommits();
    // 对列表进行过滤，只保留符合 pattern 的 commit
    List *filtered_list = filterList(L, pattern);
    // 如果只有一个 commit 符合 pattern，则直接切换到该 commit
    if (listSize(filtered_list) == 1) {
        char *commit_hash = (listGet(filtered_list, 0))->data;
        // 更新 HEAD，切换到指定 commit
        createUpdateRef("HEAD", commit_hash);
        // 恢复 worktree
        restoreCommit(commit_hash);
    } else {
        // 如果没有 commit 符合 pattern，则输出错误信息
        if (listSize(filtered_list) == 0) {
            printf("No pattern matching.\n");
        } else {
            // 如果有多个 commit 符合 pattern，则输出列表，让用户选择
            printf("Multiple matches found:\n");
            for (Cell *ptr = *filtered_list; ptr != NULL; ptr = ptr->next) {
                printf("-> %s\n", ptr->data);
            }
        }
    }
}


