#ifndef _CFGHELPER_INC
#define _CFGHELPER_INC

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
#include "util.h"

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

/* nu version */
#define NU_VERSION      "0.0.1 pre-alpha"
#define BUF_SIZE 8192

char *globNuDir;
#endif