#include "hash.c"

int main(){
    List* list = initList();
    Cell* cell = buildCell("hello");
    insertFirst(list, cell);
    cell = buildCell("world");
    insertFirst(list, cell);
    cell = buildCell("!");
    insertFirst(list, cell);
    printf("%s\n", ltos(list));
    printf("%s\n", ctos(listGet(list, 2)));
    printf("%s\n", ctos(searchList(list, "hello")));
    printf("%s\n", ltos(stol("hello|world|!")));

    ltof(list, "test.txt");
    printf("%s\n", ltos(ftol("test.txt")));

    printf("%s\n", ltos(listdir(".")));
    printf("%d\n", fileExists("listC.c"));

    cp("test2.txt", "test.txt");

    blobFile("listC.c");
    blobFile("hash.c");
    blobFile("main.c");
}