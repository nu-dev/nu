#include "pageList.h"

void pfl_add(post_frag_list *in, const char *frag) {
    post_frag_list_elem *toAdd = malloc(sizeof(post_frag_list_elem));
    toAdd->frag = strdup(frag);
    toAdd->next = NULL;
    
    /* check if this is the first post_list_elem in the dictionary */
    if (in->head == NULL) {
        in->head = toAdd;
        in->tail = toAdd;
    } else { /* not the first one */
        (in->tail)->next = toAdd;
        in->tail = toAdd;
    }
    /* incr length */
    in->length++;
}

void pfl_clean(post_frag_list *in) {
    /* get the first element */
    post_frag_list_elem *curr = in->head;
    post_frag_list_elem *next;
    
    /* loop through the entire list */
    while (curr != NULL) {
        next = curr->next;
        free(curr->frag);
        free(curr);
        curr = next;
    }
    
    free(in);
}

extern char *globNuDir;
char *calcPermalink(const char *output_fl) {
    static int offset;
    
    if (offset == 0) offset = strlen(globNuDir) + 1;
    return strdup(&output_fl[offset]);
}

post_frag_list *pfl_new() {
    /* create a new empty post_list with the right values set 
       so that it is ready to be used with other pfl_ functions */
    post_frag_list *toCreate = malloc(sizeof(post_frag_list));
    toCreate->head = NULL;
    toCreate->tail = NULL;
    toCreate->length = 0;
    return toCreate;
}
