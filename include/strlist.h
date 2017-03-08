#ifndef _STRLIST_INC
#define _STRLIST_INC

/* custom headers */
#include "common.h"

typedef struct _str_list_elem {
    char *val;
    struct _str_list_elem *next;
} str_list_elem;

typedef struct _str_list {
    str_list_elem *head;
    str_list_elem *tail;
    unsigned int length;
} str_list;

void sl_add_post(str_list *in, char *toAdd);
void sl_clean(str_list *in);
str_list *sl_new();
int sl_exists_inside(str_list *in, char *search);
#endif
