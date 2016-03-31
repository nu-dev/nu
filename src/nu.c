#include "common.h"
#include "cmds.h"

int parseOpts(int argc, char **argv) {
    if (argc == 1) {
        printf("["KRED"ERR"RESET"] Please specify an option to run. For help, use `%s help`\n", *argv);
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
            printf("["KRED"ERR"RESET"] Invalid number of arguments passed. For help, use `%s help new`\n", *argv);
        }
    } else if ((strcmp (argv[1], "build") == 0)||(strcmp (argv[1], "autobuild") == 0)) {
        
    } else if (strcmp (argv[1], "clean") == 0) {
        
    } else if (strcmp (argv[1], "cleanbuild") == 0) {
        
    } else {
        printf("["KRED"ERR"RESET"] Invalid command. For help, use `%s help`\n", *argv);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    int opt;
    opt = parseOpts(argc, argv);
    if (opt == 1) return;
}