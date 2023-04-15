#include "commit.c"

// Projet 1
// int main(){
//     List* list = initList();
//     Cell* cell = buildCell("hello");
//     insertFirst(list, cell);
//     cell = buildCell("world");
//     insertFirst(list, cell);
//     cell = buildCell("!");
//     insertFirst(list, cell);
//     printf("%s\n", ltos(list));
//     printf("%s\n", ctos(listGet(list, 2)));
//     printf("%s\n", ctos(searchList(list, "hello")));
//     printf("%s\n", ltos(stol("hello|world|!")));

//     ltof(list, "test.txt");
//     printf("%s\n", ltos(ftol("test.txt")));

//     printf("%s\n", ltos(listdir(".")));
//     printf("%d\n", fileExists("listC.c"));

//     cp("test2.txt", "test.txt");

//     blobFile("listC.c");
//     blobFile("hash.c");
//     blobFile("main.c");
// }

// Projet 2
// int main() {
    
//     WorkTree* wt = initWorkTree();

//     appendWorkTree(wt, "test.txt", NULL, 0);
//     appendWorkTree(wt, "test2.txt", NULL, 0);
//     appendWorkTree(wt, "TestForWorkTree", NULL, 0);

//     saveWorkTree(wt, "git");
//     system("echo abcd > git/test.txt");
//     restoreWorkTree(wt, "git");

// }

// Projet 3


int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "init") == 0) { // 如果第一个参数为 "init"
        initRefs(); // 初始化 .refs 目录
    }
    else if (strcmp(argv[1], "refs-list") == 0) { // 如果第一个参数为 "refs-list"
        printf("REFS:\n");
        if (fileExists(".refs")) { // 如果存在 .refs 目录
            List* L = listdir(".refs"); // 获取目录下所有文件列表
            for (Cell* ptr = *L; ptr != NULL; ptr = ptr->next) { // 遍历每个文件
                if (ptr->data[0] == '.') { // 如果文件名以 . 开头，则跳过
                    continue;
                }
                char* content = getRef(ptr->data); // 获取文件内容
                printf("- %s\t%s\n", ptr->data, content); // 打印文件名和内容
            }
        }
    }
    else if (strcmp(argv[1], "create-ref") == 0) { // 如果第一个参数为 "create-ref"
        createUpdateRef(argv[2], argv[3]); // 创建或更新引用
    }
    else if (strcmp(argv[1], "delete-ref") == 0) { // 如果第一个参数为 "delete-ref"
        deleteRef(argv[2]); // 删除引用
    }
    else if (strcmp(argv[1], "add") == 0) { // 如果第一个参数为 "add"
        for (int i = 2; i < argc; i++) { // 遍历除第一个参数外的其他参数
            myGitAdd(argv[i]); // 添加到暂存区
        }
    }
    else if (strcmp(argv[1], "clear-add") == 0) { // 如果第一个参数为 "clear-add"
        system("rm .add"); // 删除 .add 文件
    }
    else if (strcmp(argv[1], "add-list") == 0) { // 如果第一个参数为 "add-list"
        printf("Zone de preparation:\n");
        if (fileExists(".add")) { // 如果存在 .add 文件
            WorkTree* wt = ftwt(".add"); // 解析文件内容为 WorkTree 结构体
            printf("%s\n", wtts(wt)); // 打印 WorkTree 内容
        }
    }
    else if (strcmp(argv[1], "commit") == 0) { // 如果第一个参数为 "commit"
        if (strcmp(argv[3], "-m") == 0) { // 如果第三个参数为 "-m"
            myGitCommit(argv[2], argv[4]); // 提交代码，并添加提交信息
        }
        else {
            myGitCommit(argv[2], NULL); // 提交代码
        }
    }
    return 0;
}



