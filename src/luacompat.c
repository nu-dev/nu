#include "luacompat.h"

/* this file defines two C functions for use in lua :) */

extern struct NuRunConfig NuRunConfig;

/* load a lua file */
static int template_require(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);
    char *loadedfile, *tmp;
    string fileName;
    
    /* open file */
    fileName = string_printf("scripts/%s", path);
    tmp = dirJoin(NuRunConfig.themedir, fileName);
    loadedfile = dumpFile(tmp);
    free(tmp);
    string_free(fileName);
    
    /* check if there is a file */
    if (!loadedfile) return 0;
    
    /* there was a file, now run it */
    if (luaL_loadstring(L, loadedfile) != 0) {
        free(loadedfile);
        return 0;
    }
    
    free(loadedfile);
    
    /* run the code */
    if (lua_pcall(L, 0, 1, 0) != 0) {
        return 0;
    }
    
    /* top of the stack already has the result! */
    return 1;
}

/* load a template file */
static int template_include(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);
    char *loadedfile;
    string fileName, tmp, tmp2;
    
    /* open file */
    fileName = string_append("templates/", path);
    tmp = dirJoin(themedir, fileName);
    loadedfile = dumpFile(tmp);
    free(tmp);
    string_free(fileName);
    
    /* check if there is a file */
    if (!loadedfile) return 0;
    
    /* there was a file, now run it */
    tmp = goatee_gen(loadedfile);
    free(loadedfile);
    
    if (!tmp) { /* error in template */ return 0; }
    
    /* run the template, in current env (ie, without sandboxing) */
    lua_pushbool(L, true);
    tmp2 = goatee_run(L, tmp);
    string_free(tmp);
    
    if (!tmp2) { /* error in template */ return 0; }
    
    /* push string result and free it */
    lua_pushstring(L, tmp2);
    string_free(tmp2);
    
    /* top of the stack now has result */
    return 1;
}
