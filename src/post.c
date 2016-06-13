#include "post.h"

extern char *globNuDir;

typedef struct _post {
    char *name; // post name
    char *contents; // parsed contents of the post
    char cdate[100]; // date created (based on input filename)
    char mdate[100]; // date last modified
    char mtime[100]; // time last modified
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

void td_put_val(template_dictionary *in, const char *key, const char *value) {
    // set up the new template_dictionary_entry to add
    template_dictionary_entry *toAdd = malloc(sizeof(template_dictionary_entry));
    toAdd->key = strdup(key);
    toAdd->value = strdup(value);
    
    // check if this is the first template_dictionary_entry in the dictionary
    if (in->entryList == NULL) {
        in->entryList = toAdd;
        in->entryListTail = toAdd;
    } else {
        // not the first one
        (in->entryListTail)->next = toAdd;
        in->entryListTail = toAdd;
    }
    // incr length
    in->length++;
}

post *post_create(const char *in_fn) {
    char *temp;
    struct stat attr;
    time_t inTime, outTime;
    struct tm *time_tm;
    post *toCreate = malloc(sizeof(post));
    
    to->in_fn = in_fn;
    
    stat(to->in_fn, &attr);
	inTime = attr.st_mtime;
	
	if (difftime(inTime, outTime) > 0) {
	    free(toCreate);
	    return NULL;
	}
	
    to->out_loc = getOutputFileName(in_fn, globNuDir);
    
	stat(to->out_loc, &attr);
	outTime = attr.st_mtime;
	
    temp = fileName(to->out_loc);
    to->name = strndup(temp, strlen(temp) - 5); // ".html" = 5 chars
	
	time_tm = localtime(&inTime);
	strftime(to->mtime, sizeof(str_time), "%H:%M:%S", time_tm);
    strftime(to->mdate, sizeof(str_date), "%d/%m/%Y", time_tm);
}

void post_free(post *in) {
    free(in->name);
    free(in->contents);
    free(in->in_fn);
    free(in->out_loc);
    free(in);
}

void pl_clean(post_list *in) {
    // get the first entry
    post_list_entry *curr = in->head;
    post_list_entry *next;
    post *freeMe;
    
    // loop through the entire list
    while (curr != NULL) {
        next = curr->next;
        post_free(curr->me);
        free(curr);
        curr = next;
    }
    
    free(in);
}

post_list *pl_new() {
    // create a new empty post_list with the right values set
    // so that it is ready to be used with other td_ functions
    post_list *toCreate = malloc(sizeof(post_list));
    toCreate->head = NULL;
    toCreate->tail = NULL;
    toCreate->length = 0;
    return toCreate;
}