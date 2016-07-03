#ifndef _POSTUTIL_INC
#define _POSTUTIL_INC

/* custom headers */
#include "common.h"

typedef struct _post {
    char *name; /* post name */
    char *contents; /* parsed contents of the post */
    char cdate[50]; /* date created (based on input filename) */
    char mdate[50]; /* date last modified */
    char mtime[11]; /* time last modified */
    char *in_fn; /* input filename (filename ONLY, not extension) */
    char *out_loc; /* output location */
    double delta_time; /* delta time between input and output */
    int is_special; /* is this a special post */
} post;

typedef struct _post_list_elem {
    post *me;
    struct _post_list_elem *next;
} post_list_elem;

typedef struct _post_list {
    post_list_elem *head;
    post_list_elem *tail;
    unsigned int length;
} post_list;

void pl_add_post(post_list *in, post *postAdd);
post *post_create(const char *in_fpath);
void post_free(post *in);
void pl_clean(post_list *in);
post_list *pl_new();
void pl_sort(post_list **in);
#endif
