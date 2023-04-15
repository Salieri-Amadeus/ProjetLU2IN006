#include "listC.h"
#include <dirent.h>

//这个函数是用来初始化链表的
List* initList(){
    List* list = malloc(sizeof(List));
    *list = NULL;
    return list;
}

//这个函数是创建一个 Cell 类型的结构体，并将传入的字符串作为 data 字段进行初始化，
//将 next 字段初始化为 NULL。该结构体通常被用于链表的节点。
Cell* buildCell(char* ch){
    Cell* cell = malloc(sizeof(Cell));
    cell->data = strdup(ch);
    cell->next = NULL;
    return cell;
}

//在链表的头部插入一个节点
void insertFirst(List* L, Cell* C) {
    C->next = *L;
    *L = C;
}

//这个函数的作用是将链表节点（Cell）中的数据转换为字符串。
//如果节点为空，则返回空指针。
char* ctos(Cell* c){
    if (c == NULL) return NULL;
    return c->data;
}

//这个函数的作用是将一个链表中所有节点的数据连接成一个字符串，每个节点的数据之间用"|"符号隔开。
char* ltos(List* l){
    List tmp = *l;
    char* str = malloc(1000 * sizeof(char));
    str[0] = '\0';  // 初始化为一个空字符串
    while(tmp != NULL){
        if(tmp->data != NULL){
            strcat(str, ctos(tmp));
            strcat(str, "|");
        }
        tmp = tmp->next;
    }
    if(strlen(str) > 0){
        str[strlen(str)-1] = '\0'; // 去掉最后一个'|'
    }
    char *res = malloc((strlen(str) + 1) * sizeof(char)); // 加1来容纳结束符
    strcpy(res, str);
    free(str); // 释放临时字符串的内存
    return res;
}

//这个函数是用来获取链表中特定位置的元素（节点）。它接受两个参数：一个是指向链表头部的指针，一个是需要获取的节点的位置。
Cell* listGet(List* l, int i){
    List tmp = *l;
    int j = 0;
    while(tmp != NULL){
        if(j == i){
            return tmp;
        }
        tmp = tmp->next;
        j++;
    }
    return NULL;
}

//这个函数是在一个链表中搜索指定的字符串，并返回指向包含该字符串的单元格的指针。
//它接受一个指向链表的指针和一个字符串作为参数，并遍历整个链表，以查找包含指定字符串的单元格。
//如果找到，则返回该单元格的指针；否则返回NULL指针。
Cell* searchList(List* L, char* str){
    List tmp = *L;
    while(tmp != NULL){
        if(strcmp(tmp->data, str) == 0){
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

//这个函数的作用是将一个以“|”分割的字符串转化为一个链表，其中字符串中的每个子字符串都是链表中的一个节点。
List* stol(char* s) {
    List* L = initList();
    Cell* C = buildCell(s);
    insertFirst(L, C);
    char* token = strtok(NULL, "|"); // strtok的作用是将字符串分割成一系列的子字符串，每个子字符串都由分隔符分隔
    while (token != NULL) {
        C = buildCell(token);
        insertFirst(L, C);
        token = strtok(NULL, "|");
    }
    return L;
}

//用于将一个链表写入到指定路径的文件中。它的参数包括一个链表的指针l和一个路径字符串path。函数首先打开指定路径的文件，
//然后将链表转换为一个字符串，使用fprintf将该字符串写入文件中，最后关闭文件并释放内存。
void ltof(List* l, char* path){
    FILE* f = fopen(path, "w");
    if (f == NULL) {
        printf("Erreur d'ouverture\n");
        exit(1);
    }
    char* s = ltos(l);
    fprintf(f, "%s", s);
    fclose(f);
    free(s);
}

//这个函数的作用是将一个指定路径的文件中的字符串读入，然后将字符串转换成链表的形式并返回。
List* ftol(char* path){
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        printf("Erreur d'ouverture\n");
        exit(1);
    }
    char* s = malloc(1000 * sizeof(char));
    fgets(s, 1000, f);
    fclose(f);
    free(s);
    return stol(s);
}

// List * listdir(char * rootdir){
//     DIR* dp = opendir(rootdir);
//     struct dirent *ep;
//     List* list = initList();
//     if(dp == NULL){
//         printf("Erreur d'ouverture\n");
//         exit(1);
//     }
//     while((ep = readdir(dp)) != NULL){
//         Cell* cell = buildCell(ep->d_name);
//         insertFirst(list, cell);
//     }
//     closedir(dp);
//     return list;
// }

//这个函数的作用是遍历指定目录下的所有文件和子目录，将它们的名字存储在一个链表中，并返回该链表。
//函数使用了Linux系统提供的opendir和readdir函数来打开和遍历目录。
//对于每个找到的文件或目录，都创建一个新的Cell节点来保存其名称，并将该节点插入到链表的开头。函数最后返回该链表。
List* listdir(char* root_dir) {
    DIR* dp;
    struct dirent* ep;
    List* L = initList();
    *L = NULL;
    Cell* temp_cell;
    dp = opendir(root_dir);
    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            temp_cell = buildCell(ep->d_name);
            insertFirst(L, temp_cell);
            List ptr = *L;
            while (ptr != NULL) {
                ptr = ptr->next;
            }
        }
        (void)closedir(dp);
    } else {
        perror("Couldn't open the directory");
        return NULL;
    }
    return L;
}

//这个函数的作用是将一个链表中的所有节点的数据打印出来。它接受一个指向链表头部的指针作为参数，并遍历整个链表，打印每个节点的数据。
int listSize(List* L) {
    List tmp = *L;
    int i = 0;
    while (tmp != NULL) {
        i++;
        tmp = tmp->next;
    }
    return i;
}

