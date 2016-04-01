#include "cmds.h"
#include <errno.h>

#define NEWSRV_BUF_SIZE 8192
static char newsrv_buf[NEWSRV_BUF_SIZE];

static const char *defaultConfig_contents =
"# nu default config\n"
"# for help, please visit https://github.com/ohnx/nu/wiki\n"
"THEME=basic\n"
"IGNORENEWERPOST=1\n";

int newSrv(char *name) {
    int nameLen = strlen(name);
    int cwdLen = 0;
    int i;
    int startLen;
    char *addStart;
    char *dirsToMake[6] = {"", "posts", "raw", "resources", "special", "theme"};
    int dirsToMakeCount = 6;
    char *configName = "config.cfg";
    int configNameLength = strlen(configName);
    char *newDirName;
    FILE *fp;
    
    if(getCurrDir(newsrv_buf, NEWSRV_BUF_SIZE - nameLen - configNameLength) == -1) {
        return -1;
    }
    cwdLen = strlen(newsrv_buf);

    // copy over the project name
    newsrv_buf[cwdLen] = '/';
    for (i = 0; i <= nameLen; i++)
        newsrv_buf[cwdLen+i+1] = name[i];
    
    // make sure directory doesn't already exist
    if (dirExists(newsrv_buf)) {
        fprintf(stderr, "["KRED"ERR"RESET"] Directory %s already exists!\n", newsrv_buf);
        return -1;
    }
    
    for (i = 0; i < dirsToMakeCount; i++) { // loop through all the directories to make
        printf("["KBLU"INFO"RESET"] Making directory %s/%s!\n", newsrv_buf, dirsToMake[i]);
        newDirName = dirJoin(newsrv_buf, dirsToMake[i]);
        if (makeNewDir(newDirName) == 0) { // make directory itself
            fprintf(stderr, "["KRED"ERR"RESET"] Failed  to make directory %s/%s! (Error #%d)\n", newsrv_buf, dirsToMake[i], errno);
            free(newDirName);
            return -1;
        }
        free(newDirName);
    }
    
    // copy over config name
    newsrv_buf[cwdLen+nameLen+1] = '/';
    for (i = 0; i < configNameLength; i++)
        newsrv_buf[cwdLen+nameLen+i+2] = configName[i];

    // create the file
    fp = fopen(newsrv_buf, "w");
    if (fp == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to open file %s! (Error #%d)\n", newsrv_buf, errno);
        return -1;
    }
    
    // write into it
    fputs(defaultConfig_contents, fp);
    fclose(fp);
}

void cleanNuDir(char *nuDir) {
    char *removingDir;
    removingDir = dirJoin(nuDir, "posts");
    if (recursiveDelDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
    }
    free(removingDir);
    removingDir = dirJoin(nuDir, "special");
    if (recursiveDelDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
    }
    free(removingDir);
}