#include "listC.h"
#include <dirent.h>

List* initList(){
    List* list = malloc(sizeof(List));
    *list = NULL;
    return list;
}

Cell* buildCell(char* ch){
    Cell* cell = malloc(sizeof(Cell));
    cell->data = strdup(ch);
    cell->next = NULL;
    return cell;
}

void insertFirst(List* L, Cell* C) {
    C->next = *L;
    *L = C;
}


char* ctos(Cell* c){
    if (c == NULL) return NULL;
    return c->data;
}

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

