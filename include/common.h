/* external headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#define __USE_XOPEN
#define _GNU_SOURCE
#include <time.h>
/* custom headers */
#include "libnucommon.h"

#ifndef _CFGHELPER_INC
#define _CFGHELPER_INC

/* color codes */
#ifndef _NU_COLORS
    #define _NU_COLORS
    #ifdef __linux__
        #define KNRM  "\x1B[0m"
        #define KRED  "\x1B[31m"
        #define KGRN  "\x1B[32m"
        #define KYEL  "\x1B[33m"
        #define KBLU  "\x1B[34m"
        #define KMAG  "\x1B[35m"
        #define KCYN  "\x1B[36m"
        #define KWHT  "\x1B[37m"
        #define RESET "\033[0m"
    #else
        #define KNRM  ""
        #define KRED  ""
        #define KGRN  ""
        #define KYEL  ""
        #define KBLU  ""
        #define KMAG  ""
        #define KCYN  ""
        #define KWHT  ""
        #define RESET ""
    #endif
#endif

#ifdef __DEBUG
#define _okhere() do { printf("ok %d\n", __LINE__); fflush(stdout); } while(0)
#else
#define _okhere()
#endif

#define NU_CONFIG_NAME "config.kg"
#define NU_CONFIG_NAME_LENGTH 9

/* nu version */
#define NU_VERSION      "0.2.0b"
#define BUF_SIZE 8192

char *globNuDir;

typedef struct _post {
    char *name; /* post name */
    char *contents; /* parsed contents of the post */
    char cdate[50]; /* date created (based on input filename) */
    char mdate[50]; /* date last modified */
    char mtime[11]; /* time last modified */
    char *in_fn; /* input filename (filename ONLY, not extension) */
    char *out_loc; /* output location */
    char *raw_link; /* raw link */
    double delta_time; /* delta time between input and output */
    int is_special; /* is this a special post */
} post;
#endif

