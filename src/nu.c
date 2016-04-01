#include "common.h"
#include "cmds.h"

#define BUF_SIZE 8192

int parseOpts(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "["KRED"ERR"RESET"] Please specify an option to run. For help, use `%s help`\n", *argv);
        return 1;
    } else if (strcmp (argv[1], "help") == 0) {
        printf("nu version "NU_VERSION"\n");
        printf("nu is a static site generator written in C.\n");
        if (argc == 3) {
            if (strcmp (argv[2], "help") == 0) {
                
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
    } else if (strcmp (argv[1], "new") == 0) {
        if (argc == 3) {
            newSrv(argv[2]);
        } else {
            fprintf(stderr, "["KRED"ERR"RESET"] Invalid number of arguments passed. For help, use `%s help new`\n", *argv);
        }
    } else if ((strcmp (argv[1], "build") == 0)||(strcmp (argv[1], "autobuild") == 0)) {
        
    } else if (strcmp (argv[1], "clean") == 0) {
        char *nuDir;
        char * buf = calloc(BUF_SIZE, sizeof(char));
        if (argc == 2) { // no other arguments passed, assume user means current directory is nudir
            if (getCurrDir(buf, BUF_SIZE) != 0) return -1;
            nuDir = buf;
        } else if (argc == 3) { // specified a directory to clean
            if (getCurrDir(buf, BUF_SIZE) != 0) return -1;
            nuDir = dirJoin(buf, argv[2]);
            free(buf);
        } else {
            fprintf(stderr, "["KRED"ERR"RESET"] Invalid number of arguments passed. For help, use `%s help clean`\n", *argv);
            return 1;
        }
        if (!isNuDir(nuDir)) goto notnudir;
        cleanNuDir(nuDir);
        free(nuDir);
        return 0;
    notnudir:
        fprintf(stderr, "["KRED"ERR"RESET"] The specified directory %s is not a valid nu directory. Please check that the file `config.cfg` exists and try again.\n", nuDir);
        free(nuDir);
        return 1;
    } else if (strcmp (argv[1], "cleanbuild") == 0) {
        
    } else {
        fprintf(stderr, "["KRED"ERR"RESET"] Invalid command. For help, use `%s help`\n", *argv);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    return parseOpts(argc, argv);
}