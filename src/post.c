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
    time_t inTime, outTime;
    struct tm *inTime_tm, createTime;
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
	
	to->contents = parseMD(in_fpath);
	
	/* post name and creation time */
    temp = to->in_fn;
    
    if (to->is_special) {
        /* no creation date */
        /* post name is just whatever minus the HTML */
        to->name = strndup(temp, strlen(temp) - 3); /* ".md" = 3 chars */
    } else {
        /* timestamp is temp and 10 chars past */
        timestamp = strndup(temp, 10);
        
        /* post name need to remove the first bit */
        temp += 11; /* skip past 2015-06-13- (11 chars) */
        
        /* creation date */
        if (strptime(timestamp, "%Y-%m-%d", &createTime) != NULL) {
            strftime(to->cdate, 50, "%b %d, %Y", &createTime);
        } else {
            fprintf(stderr, "["KRED"ERR"RESET"] Error while parsing time. This should not happen!!\n");
            return NULL;
        }
        free(timestamp);
        
        /* post name is temp minus the html */
        to->name = strndup(temp, strlen(temp) - 3); /* ".md" = 3 chars */
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

post_list_elem* pl_merge_sort(post_list_elem *list, int (*compare)(post_list_elem *one, post_list_elem *two)) {
    post_list_elem *right,
               *temp,
               *last,
               *result,
               *next,
               *tail;
    /* Trivial case. */
    if (!list || !list->next)
        return list;

    right = list;
    temp = list;
    last = list;
    result = 0;
    next = 0;
    tail = 0;
    
    /* Find halfway through the list (by running two pointers, one at twice the speed of the other). */
    while (temp && temp->next) {
        last = right;
        right = right->next;
        temp = temp->next->next;
    }

    last->next = 0;

    /* Recurse on the two smaller lists: */
    list = pl_merge_sort(list, compare);
    right = pl_merge_sort(right, compare);

    /* Merge: */
    while (list || right) {
        /* Take from empty lists, or compare: */
        if (!right) {
            next = list;
            list = list->next;
        } else if (!list) {
            next = right;
            right = right->next;
        } else if (compare(list, right) < 0) {
            next = list;
            list = list->next;
        } else {
            next = right;
            right = right->next;
        }
        if (!result) {
            result=next;
        } else {
            tail->next=next;
        }
        next->me = tail->me;
        tail = next;
    }
    return result;
}

int _pl_cmp(post_list_elem *one, post_list_elem *two) {
    /* returns number > 0 if one is earlier than two */
    char *one_name = (one->me)->name;
    char *two_name = (two->me)->name;
    
    while (*one_name != '\0') {
        if (*one_name > *two_name) {
            return 1;
        } else if (*two_name > *one_name) {
            return -1;
        }
        /* they are equal, so keep going */
        one_name++;
        two_name++;
    }
    
    if (*two_name != '\0') {
        /* two_name is longer than one_name */
        return -1;
    }
    
    if (*one_name == *two_name) { /* both are null */
        fprintf(stderr, "["KRED"ERR"RESET"] Two posts have the same name.");
        exit(9001);
    }
    
    return 0;
}

void pl_sort(post_list *in) {
    /*post_list_elem *tmp = in->head;
    tmp = pl_merge_sort(tmp, &_pl_cmp);*/
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