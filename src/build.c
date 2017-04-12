#include "build.h"

static post_list *gl_pl;
static map_t gl_map;
static char *post_file_contents;
static char *page_file_contents;
static char *listing_file_contents;
static lua_State *Lpass;
static const char *theme;

static int dLoop(const char *inFile) {
    post *me;
    string t_out, tmp;

    if (strcmp(fileExtension(inFile), "md") && strcmp(fileExtension(inFile), "markdown")) {
        /* Not a markdown file */
        printf("["KYEL"WARN"RESET"] File `%s` does not end in `md` or `markdown` (extension `%s`); skipped.\n", inFile, fileExtension(inFile));
        return 0;
    }

    me = post_create(inFile);
    if (!me->is_special) pl_add_post(gl_pl, me); /* only add to post list if special */

    /* add the elements to map */
    hashmap_put(gl_map, "post.raw_link", me->raw_link);

    if ((tmp = parse_template(me->contents, gl_map, Lpass)) == NULL) {
        return -1;
    }

    free(me->contents);
    me->contents = tmp;

    /* add the elements to map */
    hashmap_put(gl_map, "post.name", me->name);
    hashmap_put(gl_map, "post.contents", tmp);
    if (!me->is_special)
        hashmap_put(gl_map, "post.cdate", me->cdate);
    hashmap_put(gl_map, "post.mdate", me->mdate);
    hashmap_put(gl_map, "post.mtime", me->mtime);
    hashmap_put(gl_map, "post.in_fn", me->in_fn);
    hashmap_put(gl_map, "post.out_loc", me->out_loc);
    if (me->is_special)
        hashmap_put(gl_map, "post.is_special", "true");
    
    /* parse template */
    if ((t_out = parse_template(post_file_contents, gl_map, Lpass)) == NULL) {
        return -1;
    }
    
    /* remove the elements */
    hashmap_remove(gl_map, "post.name"      );
    hashmap_remove(gl_map, "post.contents"  );
    if (!me->is_special)
        hashmap_remove(gl_map, "post.cdate"     );
    hashmap_remove(gl_map, "post.mdate"     );
    hashmap_remove(gl_map, "post.mtime"     );
    hashmap_remove(gl_map, "post.in_fn"     );
    hashmap_remove(gl_map, "post.out_loc"   );
    hashmap_remove(gl_map, "post.raw_link"  );
    if (me->is_special)
        hashmap_remove(gl_map, "post.is_special");
    
    /* add page.contents and page.title */
    tmp = t_out;
    hashmap_put(gl_map, "page.title", me->name  );
    hashmap_put(gl_map, "page.contents", tmp    );
    
    /* parse template */
    if ((t_out = parse_template(page_file_contents, gl_map, Lpass)) == NULL) {
        return -1;
    }
    
    hashmap_remove(gl_map, "page.title"     );
    hashmap_remove(gl_map, "page.contents"  );
    string_free(tmp);
    
    /* write out the file */
    writeFile(me->out_loc, t_out);
    string_free(t_out);
    
    printf("["KBLU"INFO"RESET"] Successfully parsed input file `%s`.\n", me->in_fn);
    
    if (me->is_special) post_free(me);
    return 0;
}

#if 0
static void stackDump(lua_State *L) {
    int i;
    int top = lua_gettop(L);
    for (i = -1; i >= -top; i--) {  /* repeat for each level */
      int t = lua_type(L, i);
      printf("%d: ", i);
      switch (t) {
        case LUA_TSTRING:  /* strings */
          printf("\"%s\"", lua_tostring(L, i));
          break;
    
        case LUA_TBOOLEAN:  /* booleans */
          printf(lua_toboolean(L, i) ? "true" : "false");
          break;
    
        case LUA_TNUMBER:  /* numbers */
          printf("%g", lua_tonumber(L, i));
          break;
    
        default:  /* other values */
          printf("%s", lua_typename(L, t));
          break;
    
      }
      printf("\t");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

static void table_dump(lua_State *L, int t) {
    /* table is in the stack at index 't' */
    lua_pushnil(L);  /* first key */
    while (lua_next(L, t) != 0) {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        if (lua_type(L, -2) == LUA_TSTRING) {
            printf("%s: ", lua_tostring(L, -2));
        }
        if (lua_type(L, -1) == LUA_TSTRING) {
            printf("%s", lua_tostring(L, -1));
        } else if (lua_type(L, -1) == LUA_TTABLE) {
            printf("table");
        } else {
            printf("%s", lua_typename(L, lua_type(L, -1)));
        }
        puts("");
        
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }
}
#endif

static void pushPost(lua_State *L, post *in, int n) {
    struct {
        char *key;
        char *value;
    } spt[] = {{"name", in->name},
            {"contents", in->contents},
            {"cdate", in->cdate},
            {"mdate", in->mdate},
            {"mtime", in->mtime},
            {"in_fn", in->in_fn},
            {"out_loc", in->out_loc},
            {"raw_link", in->raw_link},
            {"link", calcPermalink(in->out_loc)},
            {NULL, NULL}};
    int i;
    
    /* get env table value */
    lua_getfield(L, -1, "posts");

    if (!lua_istable(L, -1)) {
        lua_pop(L, 1); /* pop nil off */
        lua_newtable(L);
    }
    
    /* we know the table `posts` exists now at -1 */
    lua_pushnumber(L, n);

    /* now we create the child of `posts` - the # */
    lua_newtable(L);
    
    for (i = 0; 1; i++) {
        if (spt[i].key == NULL) break;
        lua_pushstring(L, spt[i].key);
        lua_pushstring(L, spt[i].value);

        if (*spt[i].key == 'l') free(spt[i].value);

        lua_settable(L, -3);
    }
    
    /* stack looks like: -1: table (new one) -2: `posts` table -3: env table */
    lua_settable(L, -3);

    /* stack looks like: -1: table (new one) -2: env table */
    lua_setfield(L, -2, "posts");
}

static void sto_ktv(lua_State *L, const char *key, const char *tkey, const char *value) {
    /* get env table value */
    lua_getfield(L, -1, key);

    if (!lua_istable(L, -1)) {
        lua_pop(L, 1); /* pop nil off */
        lua_newtable(L);
    }
    
    lua_pushstring(L, tkey);
    if (value) lua_pushstring(L, value);
    else lua_pushnil(L);
    lua_settable(L, -3);

    /* stack looks like: -1: table (new one) -2: env table */
    lua_setfield(L, -2, key);
}

static int build_pages(int ppp) {
    int max, i, totalpages;
    char totalpagesbuf[15], currpagebuf[15], buf2[15], buf3[15], pageBuf[512], *tmp2
        ,prevlink[512], nextlink[512];
    string t_out, tmp;
    post_list_elem *cptr;
    
    max = gl_pl->length;
    
    if (ppp <= 0) ppp = max;
    
    totalpages = max/ppp;
    if (totalpages*ppp < max) totalpages++; /* add that one last page */
    sprintf(totalpagesbuf, "%d", totalpages);
    hashmap_put(gl_map, "listing.totalpages", totalpagesbuf);
    
    goatee_setup_basic_table(Lpass);
    hashmap_iterate(gl_map, &hm_transfer_iter_func, Lpass);
    
    /* add the list to the env table */
    cptr = gl_pl->head; i = 1;
    while (cptr != NULL) {
        pushPost(Lpass, cptr->me, i++);
        cptr = cptr->next;
    }
    
    for (i = 1; i <= totalpages; i++) {
        /* generate string for curr page */
        sprintf(currpagebuf, "%d", i);
        sprintf(buf2, "%d", (i-1)*ppp + 1);
        sprintf(buf3, "%d", i*ppp + 1);
        sto_ktv(Lpass, "listing", "currpage", currpagebuf);
        sto_ktv(Lpass, "listing", "start", buf2);
        sto_ktv(Lpass, "listing", "finish", buf3);
        
        /* only do page links if totalpages > 1 */
        if (totalpages > 1) {
            /* first page has no previous page */
            if (i != 1) {
                if (hashmap_get(gl_map, "linkprefix", (void *)(&tmp2)) == MAP_MISSING) {
                    tmp2 = "";
                }
                sprintf(prevlink, "%s/page/%d.html", tmp2, i-1);
                sto_ktv(Lpass, "listing", "prevpage", prevlink);
            } else {
                sto_ktv(Lpass, "listing", "prevpage", NULL);
            }
            /* last page has no next page */
            if (i != totalpages) {
                if (hashmap_get(gl_map, "linkprefix", (void *)(&tmp2)) == MAP_MISSING) {
                    tmp2 = "";
                }
                sprintf(nextlink, "%s/page/%d.html", tmp2, i+1);
                sto_ktv(Lpass, "listing", "nextpage", nextlink);
            } else {
                sto_ktv(Lpass, "listing", "nextpage", NULL);
            }
        }
        
        if ((t_out = parse_template_withmap(listing_file_contents, Lpass)) == NULL) {
            fprintf(stderr, "error in template\n");
            return -1;
        }
        
        /* add page.contents and page.title */
        tmp = t_out;
        sprintf(pageBuf, "Page %d", i);
        hashmap_put(gl_map, "page.title", pageBuf   );
        hashmap_put(gl_map, "page.contents", tmp    );
        
        /* parse template */
        if ((t_out = parse_template(page_file_contents, gl_map, Lpass)) == NULL) {
            printf("error\n");
            return -1;
        }
        
        hashmap_remove(gl_map, "page.title"     );
        hashmap_remove(gl_map, "page.contents"  );
        string_free(tmp);
        
        sprintf(pageBuf, "page/%d.html", i);
        tmp2 = dirJoin(globNuDir, pageBuf);
        
        /* write out the file */
        writeFile(tmp2, t_out);
        if (i == 1) {
            free(tmp2);
            tmp2 = dirJoin(globNuDir, "index.html");
            
            writeFile(tmp2, t_out);
        }
        
        string_free(t_out);
        free(tmp2);
        printf("["KBLU"INFO"RESET"] Successfully built page %d of %d.\n", i, totalpages);
    }
        
    hashmap_remove(gl_map, "listing.totalpages");
    return 0;
}

static char *getThemeFile(char *fName, int isRequired) {
    char *tmp1, *tmp2;
    /* post file */
    tmp1 = dirJoin(globNuDir, "themes/");
    tmp2 = string_mknew(tmp1);
    free(tmp1);
    tmp2 = string_append(tmp2, string_temporary(string_mknew(fName)));

    /* read in the template file */
    tmp1 = dumpFile(tmp2);

    if (!tmp1) {
        if (isRequired) fprintf(stderr, "["KRED"ERR"RESET"] Failed to find the required theme file `%s`.\n", tmp2);
        else printf("["KYEL"WARN"RESET"] Failed to find the theme file `%s`.\n", tmp2);
        string_free(tmp2);
        return NULL;
    }
    string_free(tmp2);
    return tmp1;
}

void free_wrapper(void *in) {
    free(in);
}

int buildNuDir(const char *nuDir) {
    char *rawDir, *tmp1, *tmp2;
    const char *ppp;
    int ppp_int = -10;
    
    /* nu dir and input */
    globNuDir = strdup(nuDir);
    rawDir = dirJoin(nuDir, "raw/");
    
    /* init hashmap */
    gl_map = hashmap_new();
    
    /* read in the config */
    tmp1 = dirJoin(nuDir, "config.kg");
    tmp2 = dumpFile(tmp1);
    parse_config(tmp2, "", gl_map);
    free(tmp1);
    free(tmp2);
    
    /* get theme */
    if (hashmap_get(gl_map, "theme", (void *)(&theme)) == MAP_MISSING) {
        fprintf(stderr, "["KRED"ERR"RESET"] Could not find a key of name `theme` to determine what theme nu is going to use. Please see https://github.com/nu-dev/nu/wiki/Getting-Started for help.\n");
        return -1;
    }
    
    /* parse the theme file */
    tmp1 = getThemeFile("config.kg", 0);
    if (!tmp1) {
        fprintf(stderr, "["KYEL"WARN"RESET"] Could not find a theme config file.\n");
    } else {
        parse_config(tmp1, "theme.", gl_map);
    }
    
    /* post file */
    tmp2 = string_appendv(2, string_temporary(string_mknew(theme)), string_temporary(string_mknew("/post.html")));
    post_file_contents = getThemeFile(tmp2, 1);
    if (!post_file_contents) return -1;
    string_free(tmp2);
    
    /* page file */
    tmp2 = string_appendv(2, string_temporary(string_mknew(theme)), string_temporary(string_mknew("/page.html")));
    page_file_contents = getThemeFile(tmp2, 1);
    if (!page_file_contents) return -1;
    string_free(tmp2);
    
    /* listing file */
    tmp2 = string_appendv(2, string_temporary(string_mknew(theme)), string_temporary(string_mknew("/listing.html")));
    listing_file_contents = getThemeFile(tmp2, 1);
    if (!listing_file_contents) return -1;
    string_free(tmp2);

    /* init lua state */
    Lpass = luaL_newstate();
    luaL_openlibs(Lpass);
    
    /* build all posts and generate post list */
    gl_pl = pl_new();
    if (loopThroughDir(rawDir, &dLoop) != 0) {
        return -1;
    }
    
    /* get # of posts per page */
    if (hashmap_get(gl_map, "theme.postsperpage", (void *)(&ppp)) == MAP_MISSING) {
        fprintf(stderr, "["KYEL"WARN"RESET"] Could not find a key of name `postperpage` to determine how many posts per page. Using default.\n");
        ppp = "3";
    }
    
    ppp_int = atoi(ppp);
    
    if (ppp_int < 0) {
        fprintf(stderr, "["KYEL"WARN"RESET"] The theme %s specified a number of posts per page, but it is not a valid integer. Using default.\n", theme);
        ppp_int = 3;
    }
    
    /* build all the pages! */
    pl_sort(&gl_pl);
    if (build_pages(ppp_int) != 0) return -1;
    
    /* cleanup */
    lua_close(Lpass);
    pl_clean(gl_pl);
    hashmap_clean(gl_map, &free_wrapper);
    hashmap_free(gl_map);
    free(rawDir);
    free(globNuDir);
    free(post_file_contents);
    free(listing_file_contents);
    free(page_file_contents);
    return 0;
}
