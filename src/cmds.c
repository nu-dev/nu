#include "cmds.h"

static char newsrv_buf[BUF_SIZE];

static const char *defaultConfig_contents =
"# nu default config\n"
"# for help, please visit https://github.com/nu-dev/nu/wiki/Getting-Started\n"
"themename = \"basic\"\n"
"sitename = \"My new blog\"\n"
"sitedesc = \"My new blog!\"\n";

int newSrv(char *name) {
    #define DIRSTOMAKECOUNT 6
    int nameLen;
    int cwdLen;
    int i;
    char *dirsToMake[DIRSTOMAKECOUNT] = {"", "posts", "raw", "resources", "special", "themes"};
    char *newDirName;
    FILE *fp;
    
    nameLen = strlen(name);
    cwdLen = 0;
    
    if(getCurrDir(newsrv_buf, BUF_SIZE - nameLen - NU_CONFIG_NAME_LENGTH) == -1) {
        return -1;
    }
    cwdLen = strlen(newsrv_buf);

    /* copy over the project name */
    newsrv_buf[cwdLen] = '/';
    for (i = 0; i <= nameLen; i++)
        newsrv_buf[cwdLen+i+1] = name[i];
    
    /* make sure directory doesn't already exist */
    if (dirExists(newsrv_buf)) {
        fprintf(stderr, "["KRED"ERR"RESET"] Directory %s already exists!\n", newsrv_buf);
        return -1;
    }
    
    for (i = 0; i < DIRSTOMAKECOUNT; i++) { /* loop through all the directories to make */
        printf("["KBLU"INFO"RESET"] Making directory %s/%s!\n", newsrv_buf, dirsToMake[i]);
        newDirName = dirJoin(newsrv_buf, dirsToMake[i]);
        if (makeNewDir(newDirName) == 0) { /* make directory itself */
            fprintf(stderr, "["KRED"ERR"RESET"] Failed  to make directory %s/%s! (Error #%d)\n", newsrv_buf, dirsToMake[i], errno);
            free(newDirName);
            return -1;
        }
        free(newDirName);
    }
    
    /* copy over config name */
    newsrv_buf[cwdLen+nameLen+1] = '/';
    strncpy(&newsrv_buf[cwdLen+nameLen+2], NU_CONFIG_NAME, NU_CONFIG_NAME_LENGTH);

    /* create the file */
    fp = fopen(newsrv_buf, "w");
    if (fp == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to open file %s! (Error #%d)\n", newsrv_buf, errno);
        return -1;
    }
    
    /* write into it */
    fputs(defaultConfig_contents, fp);
    fclose(fp);
    
    #undef DIRSTOMAKECOUNT
    return 0;
}

int cleanNuDir(char *nuDir) {
    char *removingDir;
    int hasErr;
    
    hasErr = 0;
    
    removingDir = dirJoin(nuDir, "posts");
    printf("["KBLU"INFO"RESET"] Deleting directory %s...\n", removingDir);
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
        goto end;
    }
    free(removingDir); removingDir = NULL;
    
    removingDir = dirJoin(nuDir, "special");
    printf("["KBLU"INFO"RESET"] Deleting directory %s...\n", removingDir);
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
        goto end;
    }
    free(removingDir); removingDir = NULL;
    
    removingDir = dirJoin(nuDir, "page");
    printf("["KBLU"INFO"RESET"] Deleting directory %s...\n", removingDir);
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
        goto end;
    }
    free(removingDir); removingDir = NULL;
    
    removingDir = dirJoin(nuDir, "index.html");
    printf("["KBLU"INFO"RESET"] Deleting file %s...\n", removingDir);
    if (remove(removingDir) != 0) {
        fprintf(stderr, "["KYEL"WARN"RESET"] Failed to clear file %s! It may already not exist. If it does, check if you have permissions to delete.\n", removingDir);
    }
    free(removingDir); removingDir = NULL;
end:
    if (removingDir != NULL) free(removingDir);
    return hasErr;
}

char *getNuDir(int argc, char** argv) {
    char *buf;
    char *nuDir;
    
    buf = calloc(BUF_SIZE, sizeof(char));
    
    if (argc == 2) { /* no other arguments passed, assume user means current directory is nudir */
        if (getCurrDir(buf, BUF_SIZE) != 0) return NULL;
        
        nuDir = buf;
    } else if (argc == 3) { /* specified a directory to use as nudir */
        if (getCurrDir(buf, BUF_SIZE) != 0) return NULL;
        nuDir = dirJoin(buf, argv[2]);
        free(buf);
    } else {
        fprintf(stderr, "["KRED"ERR"RESET"] Invalid number of arguments passed. For help, use `%s help`.\n", *argv);
        free(buf);
        return NULL;
    }
    if (!isNuDir(nuDir)) goto notnudir;
    printf("["KBLU"INFO"RESET"] Using `%s` as the nu directory.\n", nuDir);
    return nuDir;
notnudir:
    fprintf(stderr, "["KRED"ERR"RESET"] The specified directory %s is not a valid nu directory. Please check that the file `"NU_CONFIG_NAME"` exists and try again.\n", nuDir);
    free(nuDir);
    return NULL;
}
