#include "build.h"

post_list *gl_pl;
map_t gl_map;
char *post_file_contents;

int dLoop(const char *inFile) {
    post *me;
    char *t_out;
    
    me = post_create(inFile);
    pl_add_post(gl_pl, me);
    
    /* add the elements to map */
    
    
    /* parse template */
    t_out = parse_template(post_file_contents, gl_map);
    
    /* remove the elements */
    
    
    /* write out the file */
    writeFile(me->out_loc, t_out);
    free(t_out);
    
    return 0;
}

int buildNuDir(const char *nuDir) {
    char *rawDir, *tmp1, *tmp2;
    char *theme = "default";
    
    /* nu dir and input */
    globNuDir = strdup(nuDir);
    rawDir = dirJoin(nuDir, "raw/");
    
    /* post file */
    tmp1 = dirJoin(nuDir, "themes/");
    tmp2 = string_appendv(3, string_temporary(string_mknew(tmp1)), string_temporary(string_mknew(theme)), string_temporary(string_mknew("/post.html")));
    
    free(tmp1);
    post_file_contents = dumpFile(tmp2);
    
    if (!post_file_contents) {
        printf("["KRED"ERR"RESET"] Failed to find the theme `%s`. Please check that the file %s exists.\n", theme, tmp2);
        return -1;
    }
    string_free(tmp2);
    
    /* post list */
    gl_pl = pl_new();
    loopThroughDir(rawDir, &dLoop);
    pl_sort(&gl_pl);
    
    /* cleanup */
    pl_clean(gl_pl);
    free(rawDir);
    free(globNuDir);
    free(post_file_contents);
    return 0;
}
