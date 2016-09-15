#include "post.h"

extern char *globNuDir;

void pl_add_post(post_list *in, post *postAdd) { /* set up the new post_list_elem to add */
    post_list_elem *toAdd = malloc(sizeof(post_list_elem));
    toAdd->me = postAdd;
    toAdd->next = NULL; /* check if this is the first post_list_elem in the dictionary */
    if (in->head == NULL) {
        in->head = toAdd;
        in->tail = toAdd;
    } else { /* not the first one */
        (in->tail)->next = toAdd;
        in->tail = toAdd;
    } /* incr length */
    in->length++;
}

post *post_create(const char *in_fpath) {
    const char *temp;
    char *timestamp;
    struct stat inattr, outattr;
    time_t inTime, outTime, tmpTime;
    struct tm *inTime_tm, *createTime;
    post *to = malloc(sizeof(post));
    
    /* in_fn -- make sure to get the right part */
    to->in_fn = strdup(fileName(in_fpath) + 1);
	
	/* last modified time and date */
    stat(in_fpath, &inattr);
	inTime = inattr.st_mtime;
	inTime_tm = localtime(&inTime);
	strftime(to->mtime, 11, "%H:%M:%S", inTime_tm);
    strftime(to->mdate, 50, "%B %d, %Y", inTime_tm);
	
	/* out_loc and is_special */
    to->out_loc = getOutputFileName(to->in_fn, globNuDir, &(to->is_special));
    makeFnSafe(to->out_loc);
    
    /* delta_time */
	if (stat(to->out_loc, &outattr) != 0) {
	    to->delta_time = 100;
	} else {
	    outTime = outattr.st_mtime;
        to->delta_time = difftime(inTime, outTime);
	}
	
	parseFile(in_fpath, to);
	
	if (to->contents == NULL) {
	    return NULL;
	}
    
    temp = to->in_fn;
    
    if (to->is_special) {
    } else {
        /* timestamp is temp and 10 chars past */
        timestamp = strndup(temp, 10);
        
        /* creation date */
        time(&tmpTime);
        createTime = localtime(&tmpTime);
        if (strptime(timestamp, "%Y-%m-%d", createTime) != NULL) {
            strftime(to->cdate, 50, "%b %d, %Y", createTime);
        } else {
            fprintf(stderr, "["KRED"ERR"RESET"] Error while parsing time. This should not happen!!\n");
            return NULL;
        }
        free(timestamp);
    }
    
    return to;
}

void post_free(post *in) {
    free(in->name);
    free(in->contents);
    free(in->in_fn);
    free(in->out_loc);
    free(in);
}

void pl_clean(post_list *in) {
    /* get the first element */
    post_list_elem *curr = in->head;
    post_list_elem *next;
    
    /* loop through the entire list */
    while (curr != NULL) {
        next = curr->next;
        post_free(curr->me);
        free(curr);
        curr = next;
    }
    
    free(in);
}

post_list *pl_new() {
    /* create a new empty post_list with the right values set 
       so that it is ready to be used with other td_ functions */
    post_list *toCreate = malloc(sizeof(post_list));
    toCreate->head = NULL;
    toCreate->tail = NULL;
    toCreate->length = 0;
    return toCreate;
}

int _pl_cmp(const void *one, const void *two) {
    /* returns number > 0 if one is earlier than two */
    char *one_name = (((const post_list_elem *)one)->me)->in_fn;
    char *two_name = (((const post_list_elem *)two)->me)->in_fn;
    
    while (*one_name != '\0') {
        if (*one_name > *two_name) {
            return -1;
        } else if (*two_name > *one_name) {
            return 1;
        }
        /* they are equal, so keep going */
        one_name++;
        two_name++;
    }
    
    if (*two_name != '\0') {
        /* two_name is longer than one_name */
        return 1;
    }
    
    return 0;
}

post_list_elem *_pl_make_array(post_list *in) {
    post_list_elem *arr, *tmp, *curr, *tmpref;
    
    arr = malloc(sizeof(post_list_elem) * (in->length));
    curr = in->head;
    tmp = arr;
    
    while (curr != NULL) {
        tmp->me = curr->me;
        tmpref = curr->next;
        free(curr);
        curr = tmpref;
        tmp++;
    }
    
    return arr;
}

post_list *_pl_from_array(post_list_elem *in, unsigned int length) {
    post_list_elem *created, *curr;
    post_list *lst;
    unsigned int i;
    
    lst = pl_new();
    curr = lst->head;
    
    for (i = 0; i < length; i++) {
        created = malloc(sizeof(post_list_elem));
        created->me = in[i].me;
        
        if (curr == NULL) {
            lst->head = created;
        } else {
            curr->next = created;
        }
        curr = created;
    }
    
    lst->tail = created;
    lst->length = length;
    created->next = NULL;
    
    free(in);
    return lst;
}
/*
void _pl_dump(post_list *in) {
    post_list_elem *curr;
    
    curr = in->head;
    
    while (curr != NULL) {
        printf("postlist %s\n", (curr->me)->in_fn);
        curr = curr->next;
    }
}

void _pa_dump(post_list_elem *in, unsigned int length) {
    unsigned int i;
    
    for (i = 0; i < length; i++) {
        printf("%d postarr %s\n", __LINE__, (in[i].me)->in_fn);
    }
}*/

void pl_sort(post_list **in) {
    post_list_elem *arr;
    
    /* convert the post list to an array, then quicksort it, then convert it
     * back into a list
     */
    arr = _pl_make_array(*in);

    qsort((void*)arr, (*in)->length, sizeof(post_list_elem), _pl_cmp);

    free(*in);
    *in = _pl_from_array(arr, (*in)->length);
}

/*
int main() {
    post *test;
    globNuDir = "/home/ubuntu/workspace/fs-layout/";
    test = post_create("/home/ubuntu/workspace/fs-layout/raw/2015-11-17-Test post.md");
    printf("post.name: `%s`\n", test->name);
    printf("post.contents: `%s`\n", test->contents);
    printf("post.cdate: `%s`\n", test->cdate);
    printf("post.mdate: `%s`\n", test->mdate);
    printf("post.mtime: `%s`\n", test->mtime);
    printf("post.in_fn: `%s`\n", test->in_fn);
    printf("post.out_loc: `%s`\n", test->out_loc);
    printf("post.delta_time: `%f`\n", test->delta_time);
    printf("post.is_special: `%d`\n", test->is_special);
    post_free(test);
    return 0;
}*/
