#ifndef _POSTUTIL_INC
#define _POSTUTIL_INC
#include "common.h"
#include "util.h"

/* struct _post moved to common.h */

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
