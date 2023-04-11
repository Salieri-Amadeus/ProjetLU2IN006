#include "listC.c"
#include <sys/stat.h>

//这个函数是计算给定文件的 SHA-256 哈希值，并将结果输出到指定的目标文件中。若成功返回0.
int hashfile(char* source, char* dest) {
    char cmd[1000];
    sprintf(cmd, "cat %s | openssl sha256 > %s", source, dest);
    system(cmd);
    return 0;
}

// char* sha256file(char* file){
//     system("touch tmp");
//     hashfile(file, "tmp");
//     FILE* f = fopen("tmp", "r");
//     char* hash = malloc(1000 * sizeof(char));
//     fscanf(f, "%s", hash);
//     fclose(f);
//     system("rm -rf tmp");
//     return hash;
// }

//这个函数是用于对指定文件进行 SHA-256 哈希计算的。
//它先调用了之前提到的 hashfile 函数将文件的 SHA-256 哈希值计算出来，并将结果存储到一个临时文件中。
//然后，它打开这个临时文件，读取文件中的第一行（也就是哈希值），并将其存储到一个字符缓冲区中。
//接着，它删除之前创建的临时文件，最后返回该字符缓冲区中存储的哈希值。
char* sha256file(char* file) {
    FILE* f;
    char fname[100];

    strcpy(fname, "/tmp/fileXXXXXX");
    int fd = mkstemp(fname);

    hashfile(file, fname);

    char* buffer = malloc(100 * sizeof(char));
    f = fdopen(fd, "r"); //fopen()接受文件名作为参数，而fdopen()接受文件描述符作为参数。
    fgets(buffer, 100, f);
    fclose(f);

    remove(fname);

    return buffer;
}

struct stat st = {0};

//这个函数的作用是检查给定路径（这里是当前工作文件夹）的文件是否存在。
//它使用 stat 函数检查文件状态，如果函数返回 0，则表示文件存在，否则文件不存在。
int fileExists(char* file){
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}

// int fileExists(char* file){
//     List* list = listdir(".");
//     Cell* cell = searchList(list, file);
//     if(cell == NULL){
//         return 0;
//     }
//     return 1;
// }


//这个函数是在实现一个文件复制的功能。它会接收两个参数，to和from，表示要将from文件复制到to文件中。
void cp(char *to, char *from){
    if(fileExists(from) == 0){
        printf("Le fichier n'existe pas\n");
        exit(1);
    }
    if(fileExists(to) == 1){
        char cmd[1000];
        sprintf(cmd, "touch %s", to);
        system(cmd);
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

//这个函数的作用是将哈希值转换为对应的文件路径。它将一个哈希值作为输入，并将其分为两个部分：
//前两个字符作为目录名，其余字符作为文件名。
//函数会在目录名前添加一个斜杠，以便将其视为目录。
//最后，函数会返回完整的文件路径。
char* hashToPath(char* hash){
    char* path = malloc((strlen(hash) + 1) * sizeof(char));
    strncpy(path, hash, 2);
    path[2] = '/';
    strncpy(path + 3, hash + 2, strlen(hash) - 2);
    return path;  
}

// void blobFile(char* file){
//     char* hash = sha256file(file);
//     char* path = malloc((strlen(hash) + 1 + 4) * sizeof(char));
//     strcat(path, "git/");
//     strcat(path, hashToPath(hash));
//     char* dir = malloc(2 * sizeof(char));
//     strncpy(dir, hash, 2);
//     char cmd[1000];

//     if(fileExists(path) == 0){
//         sprintf(cmd, "mkdir -p git/%s", dir);
//         system(cmd);
//         cp(path, file);
//     }

//     free(hash);
//     free(path);
// }

//这个函数实现了将一个文件转换为对应的Blob对象，首先使用sha256file()函数计算出该文件的SHA256哈希值，
//然后根据哈希值生成对应的路径，如果该路径不存在，则创建该路径，然后将该文件复制到该路径下。
//最终，该文件所对应的Blob对象就被存储在了这个路径下。
void blobFile(char* file) {
    char* hash = sha256file(file);
    char* ch2 = strdup(hash);
    ch2[2] = '\0';
    if (!fileExists(ch2)) {
        char buff[100];
        sprintf(buff, "mkdir %s", ch2);
        system(buff);
    }
    char* ch = hashToPath(hash);
    cp(ch, file);
}
