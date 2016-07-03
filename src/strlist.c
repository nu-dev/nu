#include "strlist.h"

void sl_add_post(str_list *in, char *toAdd) {
    str_list_elem *toElem = malloc(sizeof(str_list_elem));
    toElem->val = strdup(toAdd);
    toElem->next = NULL; /* check if this is the first post_list_elem in the dictionary */
    if (in->head == NULL) {
        in->head = toElem;
        in->tail = toElem;
    } else { /* not the first one */
        (in->tail)->next = toElem;
        in->tail = toElem;
    } /* incr length */
    in->length++;
}

void sl_clean(str_list *in) {
    /* get the first element */
    str_list_elem *curr = in->head;
    str_list_elem *next;
    
    /* loop through the entire list */
    while (curr != NULL) {
        next = curr->next;
        free(curr->val);
        free(curr);
        curr = next;
    }
    
    free(in);
}

str_list *sl_new() {
    str_list *toCreate = malloc(sizeof(str_list));
    toCreate->head = NULL;
    toCreate->tail = NULL;
    toCreate->length = 0;
    return toCreate;
}

int sl_exists_inside(str_list *in, char *search) {
    /* get the first element */
    str_list_elem *curr = in->head;
    
    /* loop through the entire list */
    while (curr != NULL) {
        if (strcmp(search, curr->val) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    
    return 0;
}