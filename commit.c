#include "workFile.c"

#define NcommitDefault 100
#define N 500
#define MESSAGE_SIZE 5000

typedef struct key_value_pair {
    char* key;
    char* value;
} kvp;

typedef struct hash_table {
    kvp** T;
    int n;
    int size;
} HashTable;

typedef HashTable Commit;

// 创建一个包含 key 和 value 字段的 kvp 结构体对象
kvp* createKeyVal(char* key, char* val) {
    kvp* k = malloc(sizeof(kvp)); // 分配内存
    k->key = strdup(key); // 复制 key 字符串并赋值给 k 的 key 字段
    k->value = strdup(val); // 复制 val 字符串并赋值给 k 的 value 字段
    return k;
}

// 释放 kvp 结构体对象所占用的内存
void freeKeyVal(kvp* kv) {
    free(kv->key); // 释放 key 字符串所占用的内存
    free(kv->value); // 释放 value 字符串所占用的内存
    free(kv); // 释放 kvp 结构体对象所占用的内存
}

// 将 kvp 结构体对象转换成字符串
char* kvts(kvp* k) {
    char* buff = malloc(sizeof(char) * 100); // 分配内存
    sprintf(buff, "%s : %s", k->key, k->value); // 将 k 的 key 和 value 字段格式化成字符串并保存到 buff 中
    return buff;
}

// 将字符串转换成 kvp 结构体对象
kvp* stkv(char* str) {
    char key[100], val[100];
    sscanf(str, "%s : %s", key, val); // 从字符串中解析出 key 和 value 字段的值
    return createKeyVal(key, val); // 创建一个新的 kvp 结构体对象并返回
}

// 初始化 Commit 结构体对象
Commit* initCommit(int Ncommit) {
    Commit* c = malloc(sizeof(Commit)); // 分配内存
    c->T = malloc(Ncommit * sizeof(kvp*)); // 分配内存
    c->size = Ncommit; // 初始化 size 字段
    for (int i = 0; i < c->size; i++) {
        c->T[i] = NULL; // 将所有的 kvp 指针初始化为 NULL
    }
    c->n = 0; // 初始化 n 字段
    return c;
}

unsigned long hashF(unsigned char *str)
    {
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }

// 向 Commit 结构体对象中添加一个键值对
void commitSet(Commit* c, char* key, char* value) {
    int p = hashF(key) % c->size; // 计算键 key 的哈希值并得到在哈希表中的索引 p
    while (c->T[p] != NULL) {
        p = (p + 1) % c->size; // 如果该索引已经被占用，则通过线性探测找到下一个可用的索引位置
    }
    c->T[p] = createKeyVal(key, value); // 创建一个新的 kvp 结构体对象并保存到哈希表中
    c->n++; // 增加键值对的数量
}

// 创建一个新的 Commit 结构体对象，并将一个名为 "tree" 的键值对添加到其中，该键值对的值为给定的哈希值
Commit* createCommit(char* hash) {
    Commit* c = initCommit(NcommitDefault); // 创建一个新的 Commit 结构体对象
    commitSet(c, "tree", hash); // 向 Commit 结构体对象中添加一个名为 "tree" 的键值对，值为给定的哈希值
    return c; // 返回新的 Commit 结构体对象
}

// 从 Commit 结构体对象中获取与给定键匹配的值
char* commitGet(Commit* c, char* key) {
    int p = hashF(key) % c->size; // 计算键 key 的哈希值并得到在哈希表中的索引 p
    int attempt = 0;
    while (c->T[p] != NULL && attempt < c->size) {
        if (strcmp(c->T[p]->key, key) == 0) { // 如果找到了与给定键匹配的键值对，则返回其值
            return c->T[p]->value;
        }
        p = (p + 1) % c->size; // 否则继续向下一个位置查找
        attempt = attempt + 1;
    }
    return NULL; // 如果没有找到与给定键匹配的键值对，则返回 NULL
}

// 将 Commit 结构体对象转换成字符串
char* cts(Commit* c) {
    char* str = malloc(sizeof(char) * 100 * c->n); // 分配内存
    for (int i = 0; i < c->size; i++) {
        if (c->T[i] != NULL) {
            strcat(str, kvts(c->T[i])); // 将键值对转换成字符串并保存到 str 中
            strcat(str, "\n"); // 在每个键值对之后添加一个换行符
        }
    }
    return str;
}

// 将字符串转换成 Commit 结构体对象
Commit* stc(char* ch) {
    int pos = 0;
    int n_pos = 0;
    int sep = '\n';
    char* ptr;
    char* result = malloc(sizeof(char) * 10000);
    Commit* c = initCommit(10);
    while (pos < strlen(ch)) {
        ptr = strchr(ch + pos, sep); // 在字符串中查找换行符
        if (ptr == NULL)
            n_pos = strlen(ch) + 1;
        else
            n_pos = ptr - ch + 1;
        memcpy(result, ch + pos, n_pos - pos - 1); // 将从 pos 到 n_pos-1 位置的子串复制到 result 中
        result[n_pos - pos - 1] = '\0'; // 在 result 的末尾添加一个 null 字符，以便将 result 转换成字符串
        pos = n_pos; // 更新 pos 的位置
        kvp* elem = stkv(result); // 将子串转换成键值对
        commitSet(c, elem->key, elem->value); // 将键值对添加到 Commit 结构体对象中
    }
    return c; // 返回转换好的 Commit 结构体对象
}

// 从文件中读取 Commit 结构体对象
Commit* ftc(char* file) {
    char buff[MESSAGE_SIZE];
    char* all = malloc(sizeof(char) * MESSAGE_SIZE);
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        printf("ERROR: file does not exist\n");
        return NULL;
    }
    while (fgets(buff, N, f) != NULL) {
        strcat(all, buff); // 将文件中的所有行拼接成一个字符串
    }
    Commit* c = stc(all); // 将字符串转换成 Commit 结构体对象
    return c;
}

// 将 Commit 结构体对象写入文件中
void ctf(Commit* c, char* file) {
    FILE* fp = fopen(file, "w");
    if (fp != NULL) {
        fputs(cts(c), fp); // 将 Commit 结构体对象转换成字符串并写入文件
        fclose(fp);
    }
}

// 将 Commit 结构体对象写入文件并计算其哈希值
char* blobCommit(Commit* c) {
    char fname[100] = "/tmp/myfileXXXXXX";
    int fd = mkstemp(fname); // 创建一个临时文件并返回文件描述符
    ctf(c, fname); // 将 Commit 结构体对象写入文件
    char* hash = sha256file(fname); // 计算文件的 SHA256 哈希值
    char* ch = hashToFile(hash); // 将哈希值转换成文件名
    strcat(ch, ".c");
    cp(ch, fname); // 将文件复制到以哈希值命名的文件中
    return hash; // 返回哈希值
}

// 初始化 Git 仓库中的 .refs 目录和 .refs/master 和 .refs/HEAD 文件
void initRefs() {
    if (!fileExists(".refs")) { // 如果 .refs 目录不存在，则创建该目录
        system("mkdir .refs");
        // 创建 .refs/master 文件和 .refs/HEAD 文件，并设置其内容为空
        system("touch .refs/master");
        system("touch .refs/HEAD");
    }
}

// 创建或更新 Git 仓库中的引用
void createUpdateRef(char* ref_name, char* hash) {
    char buff[100];
    sprintf(buff, "echo %s > .refs/%s", hash, ref_name); // 将哈希值写入 .refs/ref_name 文件中
    system(buff);
}

// 删除 Git 仓库中的引用
void deleteRef(char* ref_name) {
    char buff[256];
    sprintf(buff, ".refs/%s", ref_name); // 构造引用文件的路径
    if (!fileExists(buff)) { // 如果引用文件不存在，则提示该引用不存在
        printf("The reference %s does not exist.", ref_name);
    } else {
        sprintf(buff, "rm .refs/%s", ref_name); // 删除引用文件
        system(buff);
    }
}

// 获取 Git 仓库中的引用的哈希值
char* getRef(char* ref_name) {
    FILE* fp;
    char* result = malloc(sizeof(char) * 256);
    char buff[256];
    sprintf(buff, ".refs/%s", ref_name); // 构造引用文件的路径
    if (!fileExists(buff)) { // 如果引用文件不存在，则输出一条错误信息
        printf("The reference %s does not exist.", ref_name);
        return NULL;
    }
    fp = fopen(buff, "r");
    if (fp == NULL) { // 如果打开文件失败，则输出一条错误信息
        printf("Error opening file.\n");
        return NULL;
    }
    fgets(result, 256, fp); // 读取引用文件中的内容，即哈希值
    fclose(fp);
    return result;
}

// 创建文件
void createFile(char* file) {
    char buff[100];
    sprintf(buff, "touch %s", file);
    system(buff);
}

// 执行 git add 命令，将文件或文件夹加入索引区
void myGitAdd(char* file_or_folder) {
    WorkTree* wt;
    if (!fileExists(".add")) { // 判断索引文件是否存在
        createFile(".add"); // 创建索引文件
        wt = initWorkTree(); // 初始化工作树
    } else {
        wt = ftwt(".add"); // 从索引文件读取工作树信息
    }

    if (fileExists(file_or_folder)) { // 判断需要添加的文件或文件夹是否存在
        appendWorkTree(wt, file_or_folder, NULL, 0); // 将文件或文件夹添加到工作树
        wttf(wt, ".add"); // 将工作树信息写入索引文件
    } else {
        printf("file or folder %s does not exist\n", file_or_folder);
    }
}

// 执行git commit命令，将索引区的文件提交到当前分支
void myGitCommit(char* branch_name, char* message) {
    // 检查是否已经初始化了项目的引用
    printf("mmmmmmmmm");
    if (!fileExists(".refs")) {
        printf("Il faut d'abord initialiser les references du projet.");
        return;
    }
    
    char buff[256];
    sprintf(buff, ".refs/%s", branch_name);
    
    // 检查分支是否存在
    if (!fileExists(buff)) {
        printf("La branche n'existe pas.");
        return;
    }
    
    // 检查 HEAD 是否指向该分支的最新提交
    char* last_hash = getRef(branch_name);
    char* head_hash = getRef("HEAD");
    if (strcmp(last_hash, head_hash) != 0) {
        printf("HEAD doit pointer sur le dernier commit de la branche.");
        return;
    }
    
    WorkTree* wt = ftwt(".add");
    char* hashwt = saveWorkTree(wt, ".");
    Commit* c = createCommit(hashwt);
    
    // 设置父提交
    if (strlen(last_hash) != 0) {
        commitSet(c, "predecessor", last_hash);
    }
    
    // 设置提交信息
    if (message != NULL) {
        commitSet(c, "message", message);
    }
    
    char* hashc = blobCommit(c);
    createUpdateRef(branch_name, hashc);
    createUpdateRef("HEAD", hashc);
    system("rm .add");
}

