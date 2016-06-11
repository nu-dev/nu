#ifndef _CFGHELPER_INC
#define _CFGHELPER_INC

// external headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

// custom headers
#include "common.h"

typedef struct _post {
    char *name; // post name
    char *contents; // parsed contents of the post
    char *cdate; // date created (based on input filename)
    char *mdate; // date last modified
    char *mtime; // time last modified
    char *in_fn; // input filename (filename ONLY, not extension)
    char *out_loc; // output location
} post;

typedef struct _post_list_elem {
    post *me;
    struct _post_list *next;
} post_list_elem;

typedef struct _post_list {
    post_list_elem *head;
    post_list_elem *tail;
    unsigned int length;
} post_list;

#endif