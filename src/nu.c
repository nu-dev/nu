#include "common.h"
#include "cmds.h"

//static char buf[BUF_SIZE];

int parseOpts(int argc, char **argv) {
    char *nuDir;
    int hasErr = 0;
    if (argc == 1) {
        fprintf(stderr, "["KRED"ERR"RESET"] Please specify an option to run. For help, use `%s help`\n", *argv);
        return 1;
    } else if (strcmp(argv[1], "help") == 0) {
        printf("nu version "NU_VERSION"\n");
        printf("nu is a static site generator written in C.\n");
        printf("Visit https://github.com/ohnx/nu for more information.\n");
        if (argc == 3) {
            if (strcmp(argv[2], "help") == 0) {
                
            } else if (strcmp (argv[2], "new") == 0) {
                
            } else if ((strcmp (argv[2], "build") == 0)||(strcmp (argv[1], "autobuild") == 0)) {
                
            } else if (strcmp (argv[2], "clean") == 0) {
                
            } else if (strcmp (argv[2], "cleanbuild") == 0) {
                
            } else {
                goto generalcmd;
            }
        } else {
        generalcmd:
            printf("Commands for nu are as follows:\n");
            printf("`%s help`\n\tGet help for different commands\n", *argv);
        }
        return 0;
    }
    
    // individual commands
    if (strcmp(argv[1], "new") == 0) {
        if (argc == 3) {
            hasErr = newSrv(argv[2]);
        } else {
            fprintf(stderr, "["KRED"ERR"RESET"] Invalid number of arguments passed. For help, use `%s help new`\n", *argv);
            return 1;
        }
        return hasErr;
    }
    
    nuDir = getNuDir(argc, argv);
    if (nuDir == NULL) return 1;
    if ((strcmp(argv[1], "build") == 0) || (strcmp(argv[1], "autobuild") == 0)) {
        hasErr = buildNuDir(nuDir);
    } else if (strcmp(argv[1], "clean") == 0) {
        hasErr = cleanNuDir(nuDir);
    } else if (strcmp(argv[1], "cleanbuild") == 0) {
        hasErr = cleanNuDir(nuDir);
        if (hasErr) {
            fprintf(stderr, "["KRED"ERR"RESET"] Errors occured while cleaning directory. Not going to rebuild.");
            free(nuDir);
            return 1;
        }
        hasErr = buildNuDir(nuDir);
    } else {
        fprintf(stderr, "["KRED"ERR"RESET"] Invalid command. For help, use `%s help`\n", *argv);
        free(nuDir);
        return 1;
    }
    free(nuDir);
    return hasErr;
}

int main(int argc, char **argv) {
    return printf(parseOpts(argc, argv) == 0? "" : "["KRED"ERR"RESET"] Errors occured. Please check them and run again.\n");
}