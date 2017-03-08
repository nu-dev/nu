#include "common.h"
#include "cmds.h"
#include "build.h"


/*static char buf[BUF_SIZE];*/

int parseOpts(int argc, char **argv) {
    char *nuDir;
    int hasErr = 0;
    if (argc == 1) {
        fprintf(stderr, "["KRED"ERR"RESET"] Please specify an option to run. For help, use `%s help`\n", *argv);
        return 1;
    } else if (strcmp(argv[1], "help") == 0) {
        printf("nu version "NU_VERSION"\n");
        printf("nu is a static site generator written in C.\n");
        printf("Visit https://github.com/nu-dev/nu/wiki for more information.\n");
        if (argc == 3) {
            if (strcmp(argv[2], "help") == 0) {
                printf("\n%s help <command to get help on>\n", *argv);
                printf("\tGets help on the command <command to get help on>.\n");
            } else if (strcmp (argv[2], "new") == 0) {
                printf("\n%s new <folder>\n", *argv);
                printf("\tCreates a new folder named <folder> in the current directory\n");
                printf("\tand populates it with all the basic files and folders needed for nu.\n");
            } else if ((strcmp (argv[2], "build") == 0)||(strcmp (argv[1], "autobuild") == 0)) {
                printf("\n%s build <folder>\n", *argv);
                printf("\tReads all the files in <folder>/raw and attempts to parse them into\n");
                printf("\tfiles in <folder>/posts and <folder>/special.\n");
                printf("\tAlso generates <folder>/pages and <folder>/index.html.\n");
            } else if (strcmp (argv[2], "clean") == 0) {
                printf("\n%s clean <folder>\n", *argv);
                printf("\tRemoves all files in <folder>/posts, <folder>/special,\n");
                printf("\t<folder>/posts, and <folder>/index.html.\n");
            } else if (strcmp (argv[2], "cleanbuild") == 0) {
                printf("\n%s cleanbuild <folder>\n", *argv);
                printf("\tEquivalent to running `%s clean <folder>`\n", *argv);
                printf("\tthen running `%s build <folder>.\n", *argv);
            } else {
                goto generalcmd;
            }
        } else {
        generalcmd:
            printf("Commands for nu are as follows:\n");
            printf("`%s help`\n\tGet help for different commands\n", *argv);
            printf("`%s new`\n\tCreate a new nu folder\n", *argv);
            printf("`%s build`\n\tBuild all newly changed pages in a directory\n", *argv);
            printf("`%s clean`\n\tRemove all generated files\n", *argv);
            printf("`%s cleanbuild`\n\tEquivalent to cleaning then building.\n", *argv);
        }
        return 0;
    }
    
    /* individual commands */
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
            fprintf(stderr, "["KRED"ERR"RESET"] Errors occured while cleaning directory. Not going to rebuild.\n");
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
    return fprintf(stderr, parseOpts(argc, argv) == 0? "" : "["KRED"ERR"RESET"] Errors occured. Please check them and run again.\n");
}