#include "util.h"
#include <errno.h>

#define BUFSIZE 8192

// utility to check if directory exists or not
int dirExists(char *name) {
    DIR* dir = opendir(name);
    if (dir) {
        // Directory exists.
        closedir(dir);
        return 1;
    } else if (errno == ENOENT) {
        // Directory does not exist.
        return 0;
    }
    return -1;
}

int makeNewDir(char *dir) {
    int errCode;
    errCode = mkdir(dir, 0700);
    // if worked, return 1
    if (errCode == 0) return 1;
    // otherwise fallthrough to fail
    return 0;
}

char *dirJoin(char *a, char *b) {
    // join string helper function
    int lenA = strlen(a);
    // allocate memory for them
    char *newString = calloc(lenA+strlen(b)+1, sizeof(char));
    // copy over first part
    strcpy(newString, a);
    // the slash
    newString[lenA] = '/';
    // then the new folder name
    strcpy(&newString[lenA+1], b);
    return newString;
}

// get current directory
int getCurrDir(char *location, int length) {
    if (getcwd(location, length) == NULL) {
        perror("["KRED"ERR"RESET"] ");
        fprintf(stderr, "["KRED"ERR"RESET"] Could not get current directory, failing!\n");
        return -1;
    }
    return 0;
}


int isNuDir(char *dir) {
    char *configName = "config.cfg";
    FILE *fp;
    char *checkingFile = dirJoin(dir, configName);
    fp = fopen(checkingFile, "r");
    if (fp != NULL) {
        fclose(fp);
        free(checkingFile);
        return 1;
    } else {
        free(checkingFile);
        return 0;
    }
}

int recursiveDelDir(char *dirName) {
    DIR *folder = opendir(dirName);
    struct dirent *next_file;
    if (folder == NULL) {perror("["KRED"ERR"RESET"] Error opening directory"); return 1;}
    char *toDel;
    while ((next_file = readdir(folder)) != NULL) {
        if (*(next_file->d_name) == '.') continue;
        toDel = dirJoin(dirName, next_file->d_name);
        if (remove(toDel) != 0) {fprintf(stderr, "["KRED"ERR"RESET"] Error clearing file %s: ", toDel); perror(""); free(toDel); return 1;}
        free(toDel);
    }
    closedir(folder);
    return 0;
}