#include "workFile.h"
#include "hash.c"

// 以下头文件是函数isfile()所需的
#include <sys/types.h>
#include <unistd.h>

#define MAX_NB_WF 1000

int isFile(const char* path) {
    struct stat path_stat;
    if(stat(path, &path_stat) == -1)
        return 0;
    return S_ISREG(path_stat.st_mode);
}

int getChmod(const char* path) {
    struct stat ret;
    if (stat(path, &ret) == -1) {
        return -1;
    }
    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) | (ret.st_mode & S_IXUSR) | /* owner */
           (ret.st_mode & S_IRGRP) | (ret.st_mode & S_IWGRP) | (ret.st_mode & S_IXGRP) | /* group */
           (ret.st_mode & S_IROTH) | (ret.st_mode & S_IWOTH) | (ret.st_mode & S_IXOTH);  /* other */
}

void setMode(int mode, char *path) {
    char buff[100];
    sprintf(buff, "chmod %d %s", mode, path);
    system(buff);
}

WorkFile* createWorkFile(char* name){
    WorkFile* wf = malloc(sizeof(WorkFile));
    wf->name = strdup(name);
    wf->hash = NULL;
    wf->mode = 0;
    return wf;
}

char* wfts(WorkFile* wf){
    char* str = malloc(1000 * sizeof(char));
    sprintf(str, "%s\t%s\t%d", wf->name, wf->hash, wf->mode);
    char* res = malloc((strlen(str) + 1) * sizeof(char));
    strcpy(res, str);
    free(str);
    return res;
}

// WorkFile* stwf(char* ch){
//     char* name = strtok_r(ch, "\t", &ch);
//     WorkFile* wf = createWorkFile(name);
//     wf->hash = strdup(strtok_r(NULL, "\t", &ch));
//     wf->mode = atoi(strtok_r(NULL, "\t", &ch));
//     return wf;
// }

WorkFile* stwf(char* ch){
    char* name = malloc(1000 * sizeof(char));
    char* hash = malloc(1000 * sizeof(char));
    int mode;
    sscanf(ch, "%s\t%s\t%d", name, hash, &mode);
    WorkFile* wf = createWorkFile(name);
    wf->hash = strdup(hash);
    wf->mode = mode;
    return wf;
}


WorkTree* initWorkTree(){
    WorkTree* wt = malloc(sizeof(WorkTree));
    wt->tab = malloc(MAX_NB_WF * sizeof(WorkFile));
    wt->size = MAX_NB_WF;
    wt->n = 0; // 表中现有元素数量
    return wt;
}

int inWorkTree(WorkTree* wt, char* name){
    int pos = -1;
    for(int i = 0; i < wt->n; i++){
        if(strcmp(wt->tab[i].name, name) == 0){
            pos = i;
            break;
        }
    }
    return pos;
}

// int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode){
//     if(wt->n == wt->size){
//         return -1;
//     }
//     WorkFile* wf = createWorkFile(name);
//     wf->hash = strdup(hash);
//     wf->mode = mode;
//     wt->tab[wt->n] = *wf;
//     wt->n++;
//     return 0;
// }

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode){
    if(inWorkTree(wt, name) > -1){
        printf("file existe\n");
        return -3;
    }
    if(wt -> size > wt -> n){
        wt -> tab[wt -> n].name = strdup(name);
        wt -> tab[wt -> n].mode = mode;
        if(hash != NULL){
            wt -> tab[wt -> n].hash = strdup(hash);
        }
        else{
            wt -> tab[wt -> n].hash = NULL;
        }
        wt -> n++;
        return 0;
    }
    else{
        return -2;
    }
}

char* wtts(WorkTree* wt){
    char* str = malloc(1000 * sizeof(char));
    for(int i = 0; i < wt->n; i++){
        strcat(str, wfts(&wt->tab[i]));
        strcat(str, "\n");
    }
    if(strlen(str) > 0){
        str[strlen(str)-1] = '\0';
    }
    else{
        str[0] = '\0';
    }
    char* res = malloc((strlen(str) + 1) * sizeof(char));
    strcpy(res, str);
    free(str);
    return res;
}

// WorkTree* stwt(char* str){
//     WorkTree* wt = initWorkTree();
//     char* line;
//     char* saveptr;
//     line = strtok_r(str, "\n", &saveptr); // strtok_r()是线程安全的
//     while(line != NULL){
//         WorkFile* wf = stwf(line);
//         wt->tab[wt->n] = *wf;
//         wt->n++;
//         line = strtok_r(NULL, "\n", &saveptr);
//     }
//     return wt;
// }

WorkTree* stwt(char* ch){ // 答案
    int pos = 0;
    int n_pos = 0;
    int sep = '\n';
    char* ptr;
    char* result = malloc(10000 * sizeof(char));
    WorkTree* wt = initWorkTree();
    while(pos < strlen(ch)){
        ptr = strchr(ch + pos, sep);
        if(ptr == NULL){
            n_pos = strlen(ch) + 1;
        }
        else{
            n_pos = ptr - ch + 1;
        }
        memcpy(result, ch + pos, n_pos - pos - 1);
        result[n_pos - pos - 1] = '\0';
        pos = n_pos;
        WorkFile* wf = stwf(result);
        appendWorkTree(wt, wf->name, wf->hash, wf->mode);
    }
    return wt;
}

int wttf(WorkTree* wt, char* file){
    FILE* f = fopen(file, "w");
    if(f == NULL){
        return -1;
    }
    fprintf(f, "%s", wtts(wt));
    fclose(f);
    return 0;
}

WorkTree* ftwt(char* file){
    FILE* f = fopen(file, "r");
    if(f == NULL){
        return NULL;
    }
    char str[1000];
    char line[100];
    while(fgets(line, sizeof(line), f) != NULL){
        strcat(str, line);
    }
    fclose(f);
    WorkTree* wt = stwt(str);
    return wt;
}

// char* blobWorkTree(WorkTree* wt){
//     wttf(wt, "tmp.t");
//     char* hash = sha256file("tmp.t");
//     char* name = malloc(sizeof(char) * (strlen(hash) + 2));
//     strcat(name, hash);
//     strcat(name, ".t");
//     rename("tmp.t", name);
//     free(name);
//     free(hash);
//     return hash;
// }

char* hashToFile(char* hash){ // 答案
    char* ch2 = strdup(hash);
    ch2[2] = '\0';
    if(stat(ch2, &st) == -1){
        mkdir(ch2, 0700);
    }
    return hashToPath(hash);
}

char* blobWorkTree(WorkTree* wt){ // 答案
    char fname[100] = "/tmp/XXXXXX";
    int fd = mkstemp(fname);
    wttf(wt, fname);

    char* hash = sha256file(fname);
    char* ch = hashToFile(hash);
    strcat(ch, ".t");
    cp(ch, fname);
    return hash;
}

// WorkTree* dirtoWt(char* path){
//     DIR* dp = opendir(path);
//     struct dirent *ep;
//     char* hash;
//     int chmod;
//     char* name;
//     size_t path_len = strlen(path);

//     if(dp == NULL){
//         printf("Erreur d'ouverture\n");
//         exit(1);
//     }

//     if (path_len > 0 && path[path_len - 1] != '/') {
//         path = realloc(path, path_len + 2);
//         path[path_len] = '/';
//         path[path_len + 1] = '\0';
//     }

//     WorkTree* wt = initWorkTree();

//     while((ep = readdir(dp)) != NULL){
//         name = malloc(sizeof(char) * (path_len + strlen(ep->d_name) + 1));
//         strcpy(name, path);
//         strcat(name, ep->d_name);

//         if(ep->d_type == DT_REG){
//             hash = sha256file(name);
//         }
//         else{
//             hash = NULL;
//         }
//         chmod = getChmod(name);
//         appendWorkTree(wt, ep->d_name, hash, chmod);
//         free(name);
//     }

//     closedir(dp);
//     return wt;
// }


// char* saveWorkTree(WorkTree* wt, char* path){
//     for(int i = 0; i < wt->n; i++){
//         if(wt->tab[i].hash != NULL){
//             blobFile(wt->tab[i].name);
//         }
//         else{
//             WorkTree* newWT = initWorkTree();
//             DIR* dp = opendir(wt->tab[i].name);
//             struct dirent *ep;
//             char* hash;
//             int chmod;
//             if(dp == NULL){
//                 printf("Erreur d'ouverture\n");
//                 exit(1);
//             }
//             while((ep = readdir(dp)) != NULL){
//                 if(ep->d_type == DT_REG){
//                     hash = sha256file(ep->d_name);
//                 }
//                 else{
//                     hash = NULL;
//                 }
//                 chmod = getChmod(ep->d_name);
//                 appendWorkTree(newWT, ep->d_name, hash, chmod);
//             }
//             closedir(dp);
//             saveWorkTree(newWT, wt->tab[i].name);
//             return blobWorkTree(newWT);
//         }
//     }
// }

char* concat_paths(char* path1, char* path2){ // 答案
    char* result = malloc(strlen(path1) + strlen(path2) + 1);
    if(result == NULL){
        printf("Erreur d'allocation\n");
        return NULL;
    }
    strcpy(result, path1);
    strcat(result, "/");
    strcat(result, path2);
    return result;
}

char* saveWorkTree(WorkTree* wt, char* path){ // 答案
    for(int i = 0; i < wt -> n; i++){
        char* absPath = concat_paths(path, wt -> tab[i].name);
        if(isFile(absPath) == 1){
            blobFile(absPath);
            wt -> tab[i].hash = sha256file(absPath);
            wt -> tab[i].mode = getChmod(absPath);
        }
        else{
            List* list = listdir(absPath);
            WorkTree* wt2 = initWorkTree();
            for(Cell* ptr = *list; ptr != NULL; ptr = ptr -> next){
                if(ptr -> data[0] == '.'){
                    continue;
                }
                appendWorkTree(wt2, ptr -> data, NULL, 0);
                wt -> tab[i].hash = saveWorkTree(wt2, absPath);
                wt -> tab[i].mode = getChmod(absPath);
            }
        }
    }
    return blobWorkTree(wt);
}

