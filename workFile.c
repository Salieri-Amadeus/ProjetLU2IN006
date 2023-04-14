#include "workFile.h"
#include "hash.c"

// 以下头文件是函数isfile()所需的
#include <sys/types.h>
#include <unistd.h>

#define MAX_NB_WF 1000

//这个函数用于判断一个路径是否为文件（regular file），如果是返回1，否则返回0。
int isFile(const char* path) {
    struct stat path_stat;
    if(stat(path, &path_stat) == -1)
        return 0;
    return S_ISREG(path_stat.st_mode);
}

//这个函数用于获取指定路径下文件或目录的文件权限信息（即文件或目录的读、写、执行权限），返回一个整型值。
int getChmod(const char* path) {
    struct stat ret;
    if (stat(path, &ret) == -1) {
        return -1;
    }
    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) | (ret.st_mode & S_IXUSR) | /* owner */
           (ret.st_mode & S_IRGRP) | (ret.st_mode & S_IWGRP) | (ret.st_mode & S_IXGRP) | /* group */
           (ret.st_mode & S_IROTH) | (ret.st_mode & S_IWOTH) | (ret.st_mode & S_IXOTH);  /* other */
}

//这个函数用于设置指定路径下文件的权限。
void setMode(int mode, char *path) {
    char buff[100];
    sprintf(buff, "chmod %d %s", mode, path);
    system(buff);
}

//这个函数用于创建一个名为name的WorkFile结构体，其中hash和mode都初始化为0或空指针。
WorkFile* createWorkFile(char* name){
    WorkFile* wf = malloc(sizeof(WorkFile));
    wf->name = strdup(name);
    wf->hash = NULL;
    wf->mode = 0;
    return wf;
}

//这个函数的作用是将一个 WorkFile 结构体转化为字符串，以便于存储或者传输。
//它使用 sprintf 函数将 WorkFile 的 name, hash 和 mode 属性格式化为字符串，然后将这些字符串拼接起来，
//中间用制表符分隔。最后返回这个字符串的拷贝（为了防止被外部修改）。
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


//这是一个将字符串转换成WorkFile结构体的函数。它将字符串解析为三个字段：文件名、文件哈希和文件权限，
//并使用这些字段创建一个新的WorkFile结构体，并将其返回。
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

//这个函数创建了一个新的工作树，初始化了工作树的大小、元素数量，以及存储工作文件的表。
//其中，工作文件的数量被设置为 MAX_NB_WF，表示工作树能够存储的最大工作文件数量。
WorkTree* initWorkTree(){
    WorkTree* wt = malloc(sizeof(WorkTree));
    wt->tab = malloc(MAX_NB_WF * sizeof(WorkFile));
    wt->size = MAX_NB_WF;
    wt->n = 0; // 表中现有元素数量
    return wt;
}

//这个函数接收一个WorkTree类型的指针wt和一个字符串name作为参数，
//用于检查一个文件是否在工作树中，即检查一个名为name的文件是否已经被加入到了wt中。
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

//这个函数是向一个 WorkTree 中添加一个 WorkFile，其中 WorkFile 包含文件的名称、哈希值和权限。
//如果 WorkTree 中已经存在一个具有相同名称的 WorkFile，则函数返回-3；
//如果 WorkTree 已经满了，则函数返回-2；
//否则，函数向 WorkTree 中添加一个新的 WorkFile 并返回0。
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

//这个函数的作用是将工作树中的所有文件转化成一个字符串，方便写入文件或者传输。
//具体实现方式是先将工作树中每个文件的信息用wfts()函数转化成一个字符串，
//然后把所有这些字符串拼接在一起，每个字符串之间用换行符\t分隔。最后返回这个拼接后的字符串。
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

//这个函数是将字符串解析为 WorkTree 结构体类型，
//它的输入参数 ch 是一个字符串，包含了多个用换行符分隔的 WorkFile 元素信息。
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

//这个函数的作用是将 WorkTree 对象 wt 中的所有工作文件信息以文本格式写入到文件 file 中。
//如果文件成功写入，则返回 0，否则返回 -1。
int wttf(WorkTree* wt, char* file){
    FILE* f = fopen(file, "w");
    if(f == NULL){
        return -1;
    }
    fprintf(f, "%s", wtts(wt));
    fclose(f);
    return 0;
}

//这是一个用于从文件中读取字符串，然后解析为 WorkTree 结构的函数。
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


//这个函数的作用是将一个哈希值对应的文件名转换成文件路径，同时如果哈希值对应的目录不存在则创建它。
char* hashToFile(char* hash){ // 答案
    char* ch2 = strdup(hash);
    ch2[2] = '\0';
    if(stat(ch2, &st) == -1){
        mkdir(ch2, 0700);
    }
    return hashToPath(hash);
}
//这个函数和 hashToPath 函数有些类似，但是有一些不同点。
//hashToPath 函数是将一个 SHA-256 哈希值转换成对应的文件路径，
//如将 "b14a7b8059d9c055954c92674ce60032" 转换成 "b1/4a7b8059d9c055954c92674ce60032"。
//而 hashToFile 函数则是将一个 SHA-256 哈希值转换成对应的文件路径，并且还会在相应的目录中创建一个空文件。
//例如，如果传入的哈希值是 "b14a7b8059d9c055954c92674ce60032"，
//则函数会先将 "b1/4a7b8059d9c055954c92674ce60032" 作为文件路径，然后判断该路径的父目录是否存在，如果不存在就创建它。
//最后，在该路径下创建一个空文件，文件名为哈希值本身。


//这个函数的功能是将工作树中所有文件打包成一个 blob 文件，返回打包后的 blob 文件的哈希值。
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

//这个函数的作用是连接两个路径，将它们合并成一个路径，中间加上一个斜杠。
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

//这个函数的作用是将一个WorkTree中的所有文件保存到磁盘上，并返回一个表示该WorkTree的SHA-256哈希值。
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

// //这个函数的功能是释放一个WorkTree数据结构及其内部成员所占用的内存空间。
// void freeWorkTree(WorkTree* wt) {
//     if (wt == NULL) {
//         return;
//     }
//     for (int i = 0; i < wt->n; i++) {
//         free(wt->tab[i].name);
//         free(wt->tab[i].hash);
//     }
//     free(wt->tab);
//     free(wt);
// }

// //判断一个哈希值是不是一个worktree
// int isWorkTreeHash(char* hash) {
//     char* path = hashToPath(hash);
//     if(!isFile(path)) {
//         return 0;
//     }
//     WorkTree* wt = ftwt(path);
//     if(wt == NULL) {
//         return 0;
//     }
//     freeWorkTree(wt);
//     return 1;
// }

int isWorkTree(char* hash) { // 答案
  if (fileExists(strcat(hashToPath(hash), ".t"))) {
    return 1;
  }
  if (fileExists(hashToPath(hash))) {
    return 0;
  }
  return -1;
}

void restoreWorkTree(WorkTree* wt, char* path) { // 答案
  for (int i = 0; i < wt->n; i++) {
    char* absPath = concat_paths(path, wt->tab[i].name);
    char* copyPath = hashToPath(wt->tab[i].hash);
    char* hash = wt->tab[i].hash;
    if (isWorkTree(hash) == 0) { // si c’est un fichier
      cp(absPath, copyPath);
      setMode(getChmod(copyPath), absPath);
    } else {
      if (isWorkTree(hash) == 1) { // si c’est un repertoire
        strcat(copyPath, ".t");
        WorkTree* nwt = ftwt(copyPath);
        restoreWorkTree(nwt, absPath);
        setMode(getChmod(copyPath), absPath);
      }
    }
  }
}


