#include "listC.c"


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

char* hashToPath(char* hash){
    char* path = malloc((strlen(hash) + 1) * sizeof(char));
    strncpy(path, hash, 2);
    path[2] = '/';
    strncpy(path + 3, hash + 2, strlen(hash) - 2);
    return path;  
}

void blobFile(char* file){
    char* hash = sha256file(file);
    char* path = malloc((strlen(hash) + 1 + 4) * sizeof(char));
    strcat(path, "git/");
    strcat(path, hashToPath(hash));
    char* dir = malloc(2 * sizeof(char));
    strncpy(dir, hash, 2);
    char cmd[1000];

    if(fileExists(path) == 0){
        sprintf(cmd, "mkdir -p git/%s", dir);
        system(cmd);
        cp(path, file);
    }

    free(hash);
    free(path);
}
// Path: main.c 
// #include "hash.c"

// int main(int argc, char** argv){
