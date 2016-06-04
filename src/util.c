#include "util.h"

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

int delDir(char *dirName) {
    DIR *folder = opendir(dirName);
    struct dirent *next_file;
    if (folder == NULL) {perror("["KRED"ERR"RESET"] Error opening directory"); return 1;}
    char *toDel;
    while ((next_file = readdir(folder)) != NULL) {
        if (*(next_file->d_name) == '.') continue;
        toDel = dirJoin(dirName, next_file->d_name);
        if (remove(toDel) != 0) {fprintf(stderr, "["KRED"ERR"RESET"] Error clearing file %s: ", toDel); perror(""); free(toDel); return 1;}
        else {printf("["KBLU"INFO"RESET"] Removed file %s", toDel);}
        free(toDel);
    }
    closedir(folder);
    return 0;
}

int loopThroughDir(char *dirName, dirIterator iter) {
    DIR *folder = opendir(dirName);
    struct dirent *next_file;
    if (folder == NULL) {perror("["KRED"ERR"RESET"] Error opening directory"); return 1;}
    char *toRun;
    while ((next_file = readdir(folder)) != NULL) {
        if (*(next_file->d_name) == '.') continue;
        toRun = dirJoin(dirName, next_file->d_name);
        if (iter(toRun) == -1) return -1;
        free(toRun);
    }
    closedir(folder);
    return 0;
}

int fileTimeDelta(char *nameOne, char *nameTwo) {
    struct stat attr;
	time_t fileOneTime, fileTwoTime;
	
	stat(nameOne, &attr);
	fileOneTime = attr.st_mtime;
	stat(nameTwo, &attr);
	fileTwoTime = attr.st_mtime;
	
	return difftime(fileOneTime, fileTwoTime);
}

char *fileExtension(char *in) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

char *fileName(char *in) {
    return strrchr(in, '/');
}

#define isDigit(c) ((c) - '0' + 0U <= 9U)

char *getOutputFileName(char *inFile, char *nuDir) {
    char *inFileCopy = strdup(inFile);
    // get the filename
    // check if file follows post format
    //      yes, then replace dashes with '/'
    char *name = fileName(inFileCopy);
    int len = strlen(name);
    char *dotLocation;
    char *outName;
    char *fullOutName;
    // 2016-01-01-s.md we are checking that filename is long enough, we have the dashes and there are numbers
    if ((len > 15)
        && (isDigit(name[0]) && isDigit(name[1]) && isDigit(name[2]) && isDigit(name[3]) && name[4] == '-') /* year */
        && (isDigit(name[5]) && isDigit(name[6]) && name[7] == '-') /* month */
        && (isDigit(name[8]) && isDigit(name[9]) && name[10] == '-') /* day */)) { // this is a blog post
        // convert 2016-01-01-s.md to 2016/01/01/s
        dotLocation = fileExtension(name);
        *(dotLocation) = '\0';
        name[4] = '/';
        name[7] = '/';
        name[10] = '/';
        
    } else { // treat it as a special file
        
    }
}
