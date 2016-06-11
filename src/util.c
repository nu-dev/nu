#include "util.h"

// utility to check if directory exists or not
int dirExists(const char *name) {
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

int makeNewDir(const char *dir) {
    int errCode;
    errCode = mkdir(dir, 0700);
    // if worked, return 1
    if (errCode == 0) return 1;
    // otherwise fallthrough to fail
    return 0;
}

char *dirJoin(const char *a, const char *b) {
    // join string helper function
    int lenA = strlen(a);
    char *newString;
    
    // check if there isn't already a trailing "/"
    if (a[lenA-1] != '/') {
        // allocate memory for them
        newString = calloc(lenA+strlen(b)+1, sizeof(char));
        // copy over first part
        strcpy(newString, a);
        // the slash
        newString[lenA] = '/';
        // then the new folder name
        strcpy(&newString[lenA+1], b);
    } else {
        // allocate memory for them
        newString = calloc(lenA+strlen(b), sizeof(char));
        // copy over first part
        strcpy(newString, a);
        // then the new folder name
        strcpy(&newString[lenA], b);
    }
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


int isNuDir(const char *dir) {
    char *configName = "config.kg";
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

int delDir(const char *dirName) {
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

int loopThroughDir(const char *dirName, dirIterator iter) {
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

int fileTimeDelta(const char *nameOne, const char *nameTwo) {
    struct stat attr;
	time_t fileOneTime, fileTwoTime;
	
	stat(nameOne, &attr);
	fileOneTime = attr.st_mtime;
	stat(nameTwo, &attr);
	fileTwoTime = attr.st_mtime;
	
	return difftime(fileOneTime, fileTwoTime);
}

time_t getFileLastModifiedTime(const char *name) {
    struct stat attr;
	stat(name, &attr);
	return attr.st_mtime;
}

const char *fileExtension(const char *in) {
    char *dot = strrchr(in, '.');
    if(!dot || dot == in) return "";
    return dot + 1;
}

const char *fileName(const char *in) {
    return strrchr(in, '/');
}

#define isDigit(c) ((c) - '0' + 0U <= 9U)

char *getOutputFileName(const char *inFile, const char *nuDir) {
    int len;
    char *name, *outLocation, *temp, *temp2;
    
    // make a copy of the input filename so we can mutate it
    name = strdup(fileName(inFile)+1);
    len = strlen(name);
    
    // check if the filename matches the right format
    if ((len > 15)
        && (isDigit(name[0]) && isDigit(name[1]) && isDigit(name[2]) && isDigit(name[3]) && name[4] == '-') /* year */
        && (isDigit(name[5]) && isDigit(name[6]) && name[7] == '-') /* month */
        && (isDigit(name[8]) && isDigit(name[9]) && name[10] == '-') /* day */) { // this is a blog post
        // convert 2016-01-01-s.md to 2016/01/01/s
        temp = (char *)fileExtension(name) - 1;
        *temp = '\0'; // null terminate the string to remove the .md
        name[4] = '/';
        name[7] = '/';
        name[10] = '/';
        temp = strutil_append_no_mutate(name, ".html"); // add the .html
        free(name);
        temp2 = dirJoin(nuDir, "posts/"); // goes in posts dir
        outLocation = dirJoin(temp2, temp);
        free(temp);
        free(temp2);
        return outLocation;
    } else { // treat it as a special file
        temp = (char *)fileExtension(name) - 1;
        *temp = '\0'; // null terminate the string to remove the .md
        temp = strutil_append_no_mutate(name, ".html"); // add the .html
        free(name);
        temp2 = dirJoin(nuDir, "special/"); // goes in special dir
        outLocation = dirJoin(temp2, temp);
        free(temp);
        free(temp2);
        return outLocation;
    }
}

char *dumpFile(const char *filename) {
    // http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
    char * buffer = 0;
    long length;
    FILE * f = fopen (filename, "rb");
    
    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell (f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer) {
            fread (buffer, 1, length, f);
        }
        fclose (f);
    }
    
    if (buffer) {
        // start to process your data / extract strings here...
        return buffer;
    }
    return NULL;
}