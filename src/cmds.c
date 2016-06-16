#include "cmds.h"

static char newsrv_buf[BUF_SIZE];

static const char *defaultConfig_contents =
"# nu default config\n"
"# for help, please visit https://github.com/nu-dev/nu/wiki/Getting-Started\n"
"theme = \"basic\"\n"
"ignore_newer_post = \"1\"\n";

int newSrv(char *name) {
    #define DIRSTOMAKECOUNT 6
    int nameLen;
    int cwdLen;
    int i;
    char *dirsToMake[DIRSTOMAKECOUNT] = {"", "posts", "raw", "resources", "special", "theme"};
    char *newDirName;
    FILE *fp;
    
    nameLen = strlen(name);
    cwdLen = 0;
    
    if(getCurrDir(newsrv_buf, BUF_SIZE - nameLen - NU_CONFIG_NAME_LENGTH) == -1) {
        return -1;
    }
    cwdLen = strlen(newsrv_buf);

    /* copy over the project name */
    newsrv_buf[cwdLen] = '/';
    for (i = 0; i <= nameLen; i++)
        newsrv_buf[cwdLen+i+1] = name[i];
    
    /* make sure directory doesn't already exist */
    if (dirExists(newsrv_buf)) {
        fprintf(stderr, "["KRED"ERR"RESET"] Directory %s already exists!\n", newsrv_buf);
        return -1;
    }
    
    for (i = 0; i < DIRSTOMAKECOUNT; i++) { /* loop through all the directories to make */
        printf("["KBLU"INFO"RESET"] Making directory %s/%s!\n", newsrv_buf, dirsToMake[i]);
        newDirName = dirJoin(newsrv_buf, dirsToMake[i]);
        if (makeNewDir(newDirName) == 0) { /* make directory itself */
            fprintf(stderr, "["KRED"ERR"RESET"] Failed  to make directory %s/%s! (Error #%d)\n", newsrv_buf, dirsToMake[i], errno);
            free(newDirName);
            return -1;
        }
        free(newDirName);
    }
    
    /* copy over config name */
    newsrv_buf[cwdLen+nameLen+1] = '/';
    strncpy(&newsrv_buf[cwdLen+nameLen+2], NU_CONFIG_NAME, NU_CONFIG_NAME_LENGTH);

    /* create the file */
    fp = fopen(newsrv_buf, "w");
    if (fp == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to open file %s! (Error #%d)\n", newsrv_buf, errno);
        return -1;
    }
    
    /* write into it */
    fputs(defaultConfig_contents, fp);
    fclose(fp);
    
    #undef DIRSTOMAKECOUNT
    return 0;
}

int cleanNuDir(char *nuDir) {
    char *removingDir;
    int hasErr;
    
    hasErr = 0;
    
    removingDir = dirJoin(nuDir, "posts");
    printf("["KBLU"INFO"RESET"] Deleting directory %s...\n", removingDir);
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
        goto end;
    }
    freeThenNull(removingDir);
    
    removingDir = dirJoin(nuDir, "special");
    printf("["KBLU"INFO"RESET"] Deleting directory %s...\n", removingDir);
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
        goto end;
    }
    freeThenNull(removingDir);
    
    removingDir = dirJoin(nuDir, "page");
    printf("["KBLU"INFO"RESET"] Deleting directory %s...\n", removingDir);
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
        goto end;
    }
    freeThenNull(removingDir);
    
end:
    free(removingDir);
    return hasErr;
}

char *getNuDir(int argc, char** argv) {
    char *buf;
    char *nuDir;
    
    buf = calloc(BUF_SIZE, sizeof(char));
    
    if (argc == 2) { /* no other arguments passed, assume user means current directory is nudir */
        if (getCurrDir(buf, BUF_SIZE) != 0) return NULL;
        nuDir = buf;
    } else if (argc == 3) { /* specified a directory to use as nudir */
        if (getCurrDir(buf, BUF_SIZE) != 0) return NULL;
        nuDir = dirJoin(buf, argv[2]);
        free(buf);
    } else {
        fprintf(stderr, "["KRED"ERR"RESET"] Invalid number of arguments passed. For help, use `%s help clean`\n", *argv);
        free(buf);
        return NULL;
    }
    if (!isNuDir(nuDir)) goto notnudir;
    return nuDir;
notnudir:
    fprintf(stderr, "["KRED"ERR"RESET"] The specified directory %s is not a valid nu directory. Please check that the file `"NU_CONFIG_NAME"` exists and try again.\n", nuDir);
    free(nuDir);
    return NULL;
}

extern char *globNuDir;
static char *normal_template;
static char *special_template;
static char *index_template;
static char *singlepost_template;
static post_list *pl = NULL;
static post_frag_list *pfl = NULL;
static template_dictionary *combined_dic;

int builderHelper(char *inFile) {
    static post *temp;
    temp = post_create(inFile);
    if (temp == NULL) {
        return -1;
    }
    pl_add_post(pl, temp);
	return 0;
}

int buildNuDir(char *nuDir) {
    char *buildingDir = NULL, *configContents = NULL, *cfgfname = NULL,
         *temp = NULL, *themedir = NULL, *templated_output = NULL,
         *temp2 = NULL, *currpage = NULL;/*, *lastPage = NULL, *nextPage = NULL;*/
    const char *theme, *maxperpage;
    char pagenum_buf[15];/*, pagenum_buf2[15];*/
    int ok;
    unsigned int pagenum, i, maxPostsPerPage;
    template_dictionary *global_dic = NULL, *theme_dic = NULL,
                        *currpost_dic = NULL, *temp_dic = NULL;
    post_list_elem *currPost = NULL;
    post_frag_list_elem *currFrag = NULL;

    buildingDir = dirJoin(nuDir, "raw");
    globNuDir = nuDir;
    pl = pl_new();
    pfl = pfl_new();
    
    printf("["KBLU"INFO"RESET"] Starting to build the directory `%s`!\n", nuDir);
    
    /* parse global config */
    printf("["KBLU"INFO"RESET"] Parsing global nu config...\n");
    global_dic = td_new();
    cfgfname = dirJoin(nuDir, NU_CONFIG_NAME);
    configContents = dumpFile(cfgfname);
    
    if (configContents == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] Could not open the file `%s`.\n", cfgfname);
        ok = 0;
        goto end;
    }
    
    ok = parse_config(configContents, "", global_dic);
    if (!ok) {
         fprintf(stderr, "["KRED"ERR"RESET"] Errors occured while parsing the global config. Please check them and try again.\n");
         goto end;
    }
    freeThenNull(configContents);
    
    
    /* check if theme config specified */
    theme = td_fetch_val(global_dic, "theme");
    if (theme == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] Could not find a key of name `theme` to determine what theme nu is going to use. Please see https://github.com/nu-dev/nu/wiki/Getting-Started for help.\n");
        ok = 0;
        goto end;
    }
    
    temp = dirJoin(nuDir, "theme");
    themedir = dirJoin(temp, theme);
    freeThenNull(temp);
    printf("["KBLU"INFO"RESET"] Parsing theme config from %s...\n", theme);
    
    /* check if theme exists */
    if (!(dirExists(themedir) && isNuDir(themedir))) {
        fprintf(stderr, "["KRED"ERR"RESET"] The theme `%s` could not be found! Please make sure it is in the %s directory and has the file `"NU_CONFIG_NAME"`.\n", theme, themedir);
        ok = 0;
        goto end;
    }
    
    /* read theme config */
    theme_dic = td_new();
    temp = dirJoin(themedir, NU_CONFIG_NAME);
    configContents = dumpFile(temp);
    freeThenNull(temp);
    ok = parse_config(configContents, "theme.", theme_dic);
    if (!ok) {
        fprintf(stderr, "["KRED"ERR"RESET"] Errors occured while parsing the theme config. Please check them and try again.\n");
        ok = 0;
        goto end;
    }
    freeThenNull(configContents);
    
    /* read the index page for the theme */
    printf("["KBLU"INFO"RESET"] Reading index page template...\n");
    temp = dirJoin(themedir, "index.html");
    index_template = dumpFile(temp);
    if (index_template == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] The theme file `%s` could not be found! Please make sure it is in the %s directory.\n", temp, themedir);
        ok = 0;
        goto end;
    }
    freeThenNull(temp);
    
    /* read the single post fragment for the theme */
    printf("["KBLU"INFO"RESET"] Reading index page template...\n");
    temp = dirJoin(themedir, "singlepost.fragment");
    singlepost_template = dumpFile(temp);
    if (singlepost_template == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] The theme file `%s` could not be found! Please make sure it is in the %s directory.\n", temp, themedir);
        ok = 0;
        goto end;
    }
    freeThenNull(temp);
    
    /* read the post page for the theme */
    printf("["KBLU"INFO"RESET"] Reading post page template...\n");
    temp = dirJoin(themedir, "post.html");
    normal_template = dumpFile(temp);
    if (normal_template == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] The theme file `%s` could not be found! Please make sure it is in the %s directory.\n", temp, themedir);
        ok = 0;
        goto end;
    }
    freeThenNull(temp);
    
    /* read the special page for the theme */
    printf("["KBLU"INFO"RESET"] Reading special page template...\n");
    temp = dirJoin(themedir, "special.html");
    special_template = dumpFile(temp);
    if (special_template == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] The theme file `%s` could not be found! Please make sure it is in the %s directory.\n", temp, themedir);
        ok = 0;
        goto end;
    }
    freeThenNull(temp);
    
    /* combine the two dictionaries */
    combined_dic = td_merge(global_dic, theme_dic);
    
    printf("["KBLU"INFO"RESET"] Creating list of posts...\n");
    /* loop through dir and do all the stuff*/
    if (loopThroughDir(buildingDir, &builderHelper) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to build! Check if you have permissions to create files in `%s/posts` or `%s/special` and try again.\n", nuDir, nuDir);
        ok = 0;
        goto end;
    }
    
    currPost = pl->head;
    
    /* loop through the entire list */
    while (currPost != NULL) {
        if ((currPost->me)->delta_time <= 0) { /* skip this post if output file hasn't changed */
            goto nextpost;
        }
        printf("["KBLU"INFO"RESET"] Building file %s to %s...\n", (currPost->me)->in_fn, (currPost->me)->out_loc);
        /* populate the dictionary */
        temp_dic = td_new();
        td_put_val(temp_dic, "post.name", (currPost->me)->name);
        td_put_val(temp_dic, "post.contents", (currPost->me)->contents);
        td_put_val(temp_dic, "post.cdate", (currPost->me)->cdate);
        td_put_val(temp_dic, "post.mdate", (currPost->me)->mdate);
        td_put_val(temp_dic, "post.mtime", (currPost->me)->mtime);
        td_put_val(temp_dic, "post.in_fn", (currPost->me)->in_fn);
        td_put_val(temp_dic, "post.out_loc", (currPost->me)->out_loc);
        currpost_dic = td_merge(combined_dic, temp_dic);
        
        if ((currPost->me)->is_special) {
            templated_output = parse_template(special_template, currpost_dic);
        } else {
            templated_output = parse_template(normal_template, currpost_dic);

            temp = calcPermalink((currPost->me)->out_loc);
            td_put_val(currpost_dic, "post.link", temp);
            freeThenNull(temp);
            /* double pass */
            temp = parse_template(singlepost_template, currpost_dic);
            temp2 = parse_template(temp, currpost_dic);
            freeThenNull(temp);
            
            /* add post fragment */
            pfl_add(pfl, temp2);
            freeThenNull(temp2);
        }
        
        /* double pass */
        temp = templated_output;
        templated_output = parse_template(temp, currpost_dic);
        freeThenNull(temp);
        
        ok = writeFile((currPost->me)->out_loc, templated_output) + 1;
        if (!ok) {
            td_clean(temp_dic);
            goto end;
        }
        
        /* clean up */
        freeThenNull(currpost_dic);
        td_clean(temp_dic);
        
    nextpost:
        currPost = currPost->next;
    }
    
    /* create all the pages */
    /* check if theme config max posts per page */
    maxperpage = td_fetch_val(theme_dic, "theme.maxpostsperpage");
    if (maxperpage == NULL || (maxPostsPerPage = atoi(maxperpage)) == 0) {
        printf("["KYEL"WARN"RESET"] The theme %s does not specify `maxpostsperpage`, so the default value of 3 is being used instead.\n", theme);
        maxPostsPerPage = 3;
    }
    
    currFrag = pfl->head;
    i = 1;
    pagenum = 0;
    currpage = NULL;
    while (currFrag != NULL) {
        /* loop through all the posts */
        
        /* concatenate this fragment to the current string */
        if (currpage == NULL) {
            currpage = strdup(currFrag->frag);
        } else {
            temp = strutil_append_no_mutate(currpage, currFrag->frag);
            freeThenNull(currpage);
            currpage = temp;
        }
        
        /* if this is the last post on the page (postsThisPage = maxPostsPerPage)
           OR if the next page fragment == NULL
           then dump the current string to the page `n` */
        if (i == maxPostsPerPage || currFrag->next == NULL) {
            /* get page output */
            temp = dirJoin(nuDir, "page");
            sprintf(pagenum_buf, "%d.html", pagenum);
            temp2 = dirJoin(temp, pagenum_buf);
            freeThenNull(temp);
            printf("["KBLU"INFO"RESET"] Building page %d to %s...\n", pagenum, temp2);
            
            /* temp post dic */
            temp_dic = td_new();
            td_put_val(temp_dic, "pagination.currpage", currpage);
            currpost_dic = td_merge(combined_dic, temp_dic);
            
            /* double pass */
            temp = parse_template(index_template, currpost_dic);
            templated_output = parse_template(temp, currpost_dic);
            freeThenNull(temp);
            
            ok = writeFile(temp2, templated_output) + 1;
            if (!ok) {
                td_clean(temp_dic);
                goto end;
            }
            td_clean(temp_dic);
            if (pagenum == 0) {
                /* also write the index.html */
                temp = dirJoin(nuDir, "index.html");
                ok = writeFile(temp, templated_output) + 1;
                if (!ok) {
                    goto end;
                }
                freeThenNull(temp);
            }
            
            i = 1;
            pagenum++;
            freeThenNull(currpage);
            freeThenNull(currpost_dic);
            freeThenNull(templated_output);
        } else {
            i++;
        }
        currFrag = currFrag->next;
    }
    
    end:
    if (pl) pl_clean(pl);
    if (pfl) pfl_clean(pfl);
    free(buildingDir);
    free(configContents);
    free(cfgfname);
    free(temp);
    free(temp2);
    free(themedir);
    free(currpage);
    free(theme_dic);
    free(global_dic);
    free(templated_output);
    free(normal_template);
    free(special_template);
    free(index_template);
    free(singlepost_template);
    return !ok;
}