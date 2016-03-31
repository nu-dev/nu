#include "cmds.h"
#include <errno.h>

#define NEWSRV_BUF_SIZE 8192
static char buf[NEWSRV_BUF_SIZE];

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
    FILE *fp;
    
    // get current directory
    if (getcwd(buf, NEWSRV_BUF_SIZE - nameLen - configNameLength) == NULL) {
        if (errno == EINVAL) {
            printf("["KRED"ERR"RESET"] EINVAL - Interal error.\n");
        } else if (errno == ERANGE) {
            printf("["KRED"ERR"RESET"] ERANGE - The current directory name is too long!\n");
        } else if (errno == EACCES) {
            printf("["KRED"ERR"RESET"] EACCES - No permissions to get directory name.\n");
        }
        printf("["KRED"ERR"RESET"] Could not get current directory, failing!\n");
        return -1;
    }
    cwdLen = strlen(buf);

    // copy over the project name
    buf[cwdLen] = '/';
    for (i = 0; i <= nameLen; i++)
        buf[cwdLen+i+1] = name[i];
    
    // make sure directory doesn't already exist
    if (dirExists(buf)) {
        printf("["KRED"ERR"RESET"] Directory %s already exists!\n", buf);
        return -1;
    }
    
    for (i = 0; i < dirsToMakeCount; i++) { // loop through all the directories to make
        printf("["KBLU"INFO"RESET"] Making directory %s/%s!\n", buf, dirsToMake[i]);
        if (makeNewDir(buf, dirsToMake[i]) == 0) { // make directory itself
            printf("["KRED"ERR"RESET"] Failed  to make directory %s/%s! (Error #%d)\n", buf, dirsToMake[i], errno);
            return -1;
        }
    }
    
    // copy over config name
    buf[cwdLen+nameLen+1] = '/';
    fflush(stdout);
    for (i = 0; i < configNameLength; i++)
        buf[cwdLen+nameLen+i+2] = configName[i];

    // create the file
    fp = fopen(buf, "w");
    if (fp == NULL) {
        printf("["KRED"ERR"RESET"] Failed to open file %s! (Error #%d)\n", buf, errno);
        return -1;
    }
    
    // write into it
    fputs(defaultConfig_contents, fp);
    fclose(fp);
}