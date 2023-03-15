#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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