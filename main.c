#include "workFile.c"

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
int main(void) {
    // 创建一个WorkTree，添加一些WorkFile
    WorkTree* wt = initWorkTree();
    appendWorkTree(wt, "file1.txt", "hash1", 644);
    appendWorkTree(wt, "file2.txt", "hash2", 600);
    appendWorkTree(wt, "file3.txt", "hash3", 755);
    appendWorkTree(wt, "file4.txt", "hash4", 444);

    WorkFile* w1 = createWorkFile("listC.c");
    w1->hash = sha256file("listC.c");
    w1->mode = getChmod("listC.c");

    // 将WorkTree保存到文件
    wttf(wt, "worktree.txt");

    // 从文件中读取WorkTree
    WorkTree* wt2 = ftwt("worktree.txt");

    // 打印两个WorkTree
    printf("WorkTree 1:\n%s\n", wtts(wt));
    printf("WorkTree 2:\n%s\n", wtts(wt2));

    blobWorkTree(wt2);

    WorkTree* test = dirtoWt("workTreeTest");
    wttf(test, "worktreeTest.txt");


    return 0;
}

