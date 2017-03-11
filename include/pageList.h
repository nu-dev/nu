#ifndef _PAGELIST_INC
#define _PAGELIST_INC

/* custom headers */
#include "common.h"

typedef struct _post_frag_list_elem {
    char *frag;
    struct _post_frag_list_elem *next;
} post_frag_list_elem;

typedef struct _post_frag_list {
    post_frag_list_elem *head;
    post_frag_list_elem *tail;
    unsigned int length;
} post_frag_list;

char *calcPermalink(const char *output_fl);
void pfl_add(post_frag_list *in, const char *frag);
void pfl_clean(post_frag_list *in);
post_frag_list *pfl_new();
#endif
