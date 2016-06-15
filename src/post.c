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
    
    /* delta_time */
	if (stat(to->out_loc, &outattr) != 0) {
	    to->delta_time = -1;
	} else {
	    outTime = outattr.st_mtime;
        to->delta_time = difftime(inTime, outTime);
	}
	
	to->contents = parseMD(in_fpath);
	/* parsed contents - only parse if difftime > 0. otherwise,  
    if (to->delta_time >= 0) {
        to->contents = parseMD(in_fpath);
        if (to->contents == NULL) {
            error while parsing
            return NULL;
        }
    } else {
        to->contents = NULL;
    }*/
	
	/* post name and creation time */
    temp = to->in_fn;
    
    if (to->is_special) {
        /* no creation date */
        /* post name is just whatever minus the HTML */
        to->name = strndup(temp, strlen(temp) - 5); /* ".html" = 5 chars */
    } else {
        /* timestamp is temp and 10 chars past */
        timestamp = strndup(temp, 10);
        
        /* post name need to remove the first bit */
        temp += 11; /* skip past 2015-06-13- (11 chars) */
        
        /* creation date */
        if (strptime(timestamp, "%Y-%m-%d", &createTime) != NULL) {
            strftime(to->cdate, 50, "%B %d, %Y", &createTime);
        } else {
            fprintf(stderr, "["KRED"ERR"RESET"] Error while parsing time. This should not happen!!\n");
            return NULL;
        }
        free(timestamp);
        
        /* post name is temp minus the html */
        to->name = strndup(temp, strlen(temp) - 3); /* ".html" = 5 chars */
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