#include "listC.h"
#include "hash.c"
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

// char* ltos(List* l){
//     List tmp = *l;
//     char* str = malloc(1000 * sizeof(char));
//     while(tmp->next != NULL){
//         if(tmp->data != NULL){
//             strcat(str, ctos(tmp));
//             strcat(str, "|");
//         }
//         tmp = tmp->next;
//     }
//     char *res = malloc((strlen(str) - 1) * sizeof(char));
//     res = strncpy(res, str, strlen(str) - 1);
//     return res;
// }

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

List * listdir(char * rootdir){
    DIR* dp = opendir(rootdir);
    struct dirent *ep;
    List* list = initList();
    if(dp == NULL){
        printf("Erreur d'ouverture\n");
        exit(1);
    }
    while((ep = readdir(dp)) != NULL){
        Cell* cell = buildCell(ep->d_name);
        insertFirst(list, cell);
    }
    closedir(dp);
    return list;
}

int fileExists(char* file){
    List* list = listdir(".");
    Cell* cell = searchList(list, file);
    if(cell == NULL){
        return 0;
    }
    return 1;
}

void cp(char *to, char *from){
    if(fileExists(from) == 0){
        printf("Le fichier n'existe pas\n");
        exit(1);
    }
    FILE* src = fopen(from, "r");
    FILE* dest = fopen(to, "w");

    if(src == NULL || dest == NULL){
        printf("Erreur d'ouverture\n");
        exit(1);
    }

    char* s = malloc(1000 * sizeof(char));

    while(fgets(s, 1000, src) != NULL){
        fputs(s, dest);
    }

    fclose(src);
    fclose(dest);
    free(s);
}

char* hashToPath(char* hash){
    char* path = malloc((strlen(hash) + 1) * sizeof(char));
    strncpy(path, hash, 2);
    path[2] = '/';
    strncpy(path + 3, hash + 2, strlen(hash) - 2);
    return path;  
}

void blobFile(char* file){
    char* hash = sha256file(file);
    char* path = hashToPath(hash);
    char* dir = malloc(2 * sizeof(char));
    strncpy(dir, path, 2);
    char cmd[1000];

    if(fileExists(path) == 0){
        sprintf(cmd, "mkdir -p %s", dir);
        system(cmd);
        cp(path, file);
    }

    free(hash);
    free(path);
}
