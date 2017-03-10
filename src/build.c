#include "build.h"

static post_list *gl_pl;
static map_t gl_map;
static char *post_file_contents;
static char *page_file_contents;
static lua_State *Lpass;

static int dLoop(const char *inFile) {
    post *me;
    string t_out, tmp;
    
    me = post_create(inFile);
    pl_add_post(gl_pl, me);
    
    if ((tmp = parse_template(me->contents, gl_map, Lpass)) == NULL) {
        return -1;
    }
    
    printf("----- PARSED POST SUCCESSFULLY!!! OUTPUT FROM THAT: -----\n%s\n------", tmp);
    
    /* add the elements to map */
    hashmap_put(gl_map, "post.name", me->name);
    hashmap_put(gl_map, "post.contents", tmp);
    hashmap_put(gl_map, "post.mdate", me->mdate);
    hashmap_put(gl_map, "post.mtime", me->mtime);
    hashmap_put(gl_map, "post.in_fn", me->in_fn);
    hashmap_put(gl_map, "post.out_loc", me->out_loc);
    hashmap_put(gl_map, "post.raw_link", me->raw_link);
    
    /* parse template */
    if ((t_out = parse_template(post_file_contents, gl_map, Lpass)) == NULL) {
        return -1;
    }
    
    /* remove the elements */
    hashmap_remove(gl_map, "post.name"      );
    hashmap_remove(gl_map, "post.contents"  );
    string_free(tmp);
    hashmap_remove(gl_map, "post.mdate"     );
    hashmap_remove(gl_map, "post.mtime"     );
    hashmap_remove(gl_map, "post.in_fn"     );
    hashmap_remove(gl_map, "post.out_loc"   );
    hashmap_remove(gl_map, "post.raw_link"  );
    
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
    
    return 0;
}

static char *getThemeFile(char *fName) {
    char *tmp1, *tmp2;
    /* post file */
    tmp1 = dirJoin(globNuDir, "themes/");
    tmp2 = string_appendv(2, tmp1, fName);
    free(tmp1);

    /* read in the template file */
    tmp1 = dumpFile(tmp2);
    
    if (!tmp1) {
        printf("["KRED"ERR"RESET"] Failed to find the required theme file `%s`.\n", tmp2);
        return NULL;
    }
    string_free(tmp2);
    return tmp1;
}

int buildNuDir(const char *nuDir) {
    char *rawDir, *tmp1, *tmp2;
    const char *theme;
    
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
    
    /* post file */
    tmp2 = string_appendv(2, string_temporary(string_mknew(theme)), string_temporary(string_mknew("/post.html")));
    post_file_contents = getThemeFile(tmp2);
    if (!post_file_contents) return -1;
    string_free(tmp2);
    
    /* page file */
    tmp2 = string_appendv(2, string_temporary(string_mknew(theme)), string_temporary(string_mknew("/page.html")));
    page_file_contents = getThemeFile(tmp2);
    if (!page_file_contents) return -1;
    string_free(tmp2);

    /* init lua state */
    Lpass = luaL_newstate();
    luaL_openlibs(Lpass);
    
    /* post list */
    gl_pl = pl_new();
    if (loopThroughDir(rawDir, &dLoop) != 0) {
        return -1;
    }
    pl_sort(&gl_pl);
    
    /* cleanup */
    lua_close(Lpass);
    pl_clean(gl_pl);
    free(rawDir);
    free(globNuDir);
    free(post_file_contents);
    return 0;
}
