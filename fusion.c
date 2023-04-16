#include "branch.c"

WorkFile* getWorkFile(WorkTree* wt, char* path) {
    for (int i=0; i<wt->n; i++) {
        WorkFile* wf = &wt->tab[i];
        if (strcmp(wf->name, path) == 0) return wf;
    }
    return NULL;
}

WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts){
    WorkTree *merged = initWorkTree();
    for (int i = 0; i < wt1->n; i++) {
        WorkFile *file = wt1->tab + i;
        WorkFile *file2 = getWorkFile(wt2, file->name);
        if (file2 == NULL) {
            appendWorkTree(merged, file->name, file->hash, file->mode);
        } 
        else {
            if (strcmp(file->hash, file2->hash) == 0) {
                appendWorkTree(merged, file->name, file->hash, file->mode);
            } 
            else {
                insertFirst(conflicts, buildCell(file->name));
            }
        }
    }
    for (int i = 0; i < wt2->n; i++) {
        WorkFile *file = wt2->tab + i;
        WorkFile *file1 = getWorkFile(wt1, file->name);
        if (file1 == NULL) {
            appendWorkTree(merged, file->name, file->hash, file->mode);
        }
    }
    return merged;
}

WorkTree* getWorkTree(char* commit_hash) {
    char* commit_path = hashToPath(commit_hash);
    strcat(commit_path, ".c");
    Commit* commit = ftc(commit_path);
    char* wt_path = hashToPath(commitGet(commit, "tree"));
    strcat(wt_path, ".t");
    WorkTree* wt = ftwt(wt_path);
    return wt;
}


List* merge(char* remote_branch, char* message) {
    char* current_branch = getCurrentBranch();
    char* commit_hash = getRef(current_branch);
    char* remote_commit_hash = getRef(remote_branch);
    WorkTree* wt1 = getWorkTree(commit_hash);
    WorkTree* wt2 = getWorkTree(remote_commit_hash);
    List* conflicts = initList();
    WorkTree* wt = mergeWorkTrees(wt1, wt2, &conflicts);
        if (listSize(conflicts) == 0) {
        char* hashwt = saveWorkTree(wt, ".");
        Commit* c = createCommit(hashwt);
        commitSet(c, "predecessor", commit_hash);
        commitSet(c, "merge_predecessor", remote_commit_hash);
        if(message==NULL){
            commitSet(c, "message", NULL);
        }
        else{
            commitSet(c, "message", message);
        }
        char* hashc = blobCommit(c);
        createUpdateRef(current_branch, hashc);
        createUpdateRef("HEAD", hashc);
        deleteRef(remote_branch);
        restoreCommit(hashc);
        return NULL;
    }
    else{
        return conflicts;
    }
}

void createDeletionCommit(char* branch, List* conflicts, char* message){
    char* starting_branch = getCurrentBranch();
    if (listSize(conflicts) == 0){
        printf("Error : no conflicts, exiting.%s\n", ltos(conflicts));
        exit(1); 
    }
    if (!branchExists(branch)){
        printf("Error : Branch '%s' does not exist, exiting.\n", branch);
        exit(1); 
    }
    char* commit_hash = getRef(branch);
    WorkTree* wt = getWorkTree(commit_hash);
    free(commit_hash);
    if(fileExists(".add")){
        system("rm .add");
    }
    for (int i=0; i<wt->n; ++i) {
        WorkFile* wf = &wt->tab[i];
        if (searchList(conflicts, wf->name)==NULL)
            myGitAdd(wf->name);
    }
    myGitCheckoutBranch(branch);
    myGitCommit(branch, message);
    myGitCheckoutBranch(starting_branch);
}