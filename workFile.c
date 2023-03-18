#include "workFile.h"
#include "hash.c"
#include <sys/stat.h>

#define MAX_NB_WF 1000

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
//     char* name = strtok(ch, "\t");
//     WorkFile* wf = createWorkFile(name);
//     wf->hash = strdup(strtok(NULL, "\t"));
//     wf->mode = atoi(strtok(NULL, "\t"));
//     return wf;
// }

WorkFile* stwf(char* ch){
    char* name = strtok_r(ch, "\t", &ch);
    WorkFile* wf = createWorkFile(name);
    wf->hash = strdup(strtok_r(NULL, "\t", &ch));
    wf->mode = atoi(strtok_r(NULL, "\t", &ch));
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

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode){
    if(wt->n == wt->size){
        return -1;
    }
    WorkFile* wf = createWorkFile(name);
    wf->hash = strdup(hash);
    wf->mode = mode;
    wt->tab[wt->n] = *wf;
    wt->n++;
    return 0;
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
//     char* line = strtok(str, "\n");
//     while(line != NULL){
//         WorkFile* wf = stwf(line);
//         wt->tab[wt->n] = *wf;
//         wt->n++;
//         line = strtok(NULL, "\n");
//     }
//     return wt;
// }

WorkTree* stwt(char* str){
    WorkTree* wt = initWorkTree();
    char* line;
    char* saveptr;
    line = strtok_r(str, "\n", &saveptr); // strtok_r()是线程安全的
    while(line != NULL){
        WorkFile* wf = stwf(line);
        wt->tab[wt->n] = *wf;
        wt->n++;
        line = strtok_r(NULL, "\n", &saveptr);
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
