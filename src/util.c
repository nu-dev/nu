#include "util.h"
#include <errno.h>

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

int makeNewDir(char *curr, char *newPart) {
    int lenCurr = strlen(curr);
    // allocate memory for them
    char *newStr = calloc(lenCurr + strlen(newPart) + 1, sizeof(char));
    // copy over first part
    strcpy(newStr, curr);
    // the slash
    newStr[lenCurr] = '/';
    // then the new folder name
    strcpy(&newStr[lenCurr+1], newPart);
    // make the directory and check the status
    lenCurr = mkdir(newStr, 0700);
    // if worked, goto good
    if (lenCurr == 0) goto good;
    // otherwise fallthrough to fail
fail:
    free(newStr);
    return 0;
good:
    free(newStr);
    return 1;
}

