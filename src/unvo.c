#include "unvo.h"

int hm_transfer_iter_func(void *luaRef, const char *key, void *value) {
    lua_State *L = (lua_State *)luaRef;
    char *keyFirst, *keySecond, *dotLocation;
    
    dotLocation = strchr(key, '.');
    /* handle cases where we need a table */
    if (dotLocation != NULL) {
        keyFirst = strdup(key);
        keySecond = keyFirst + (dotLocation-key);
        
        /* set the dot to null */
        *(keySecond++) = '\0';
        
        /* get env table value */
        lua_getfield(L, 4, keyFirst);
        
        if (!lua_istable(L, -1)) {
            /* need to create the table 
            printf("creating table `%s` to insert key `%s`\n", keyFirst, key);*/
            lua_newtable(L);
            
            /* insert the value 
            printf("inserting into table `%s` key `%s` value `%.5s`\n", keyFirst, keySecond, (char *)value);*/
            lua_pushstring(L, keySecond);
            lua_pushstring(L, value);
            
            lua_settable(L, -3);
            
            /* expect stack to be <table> <env>*/
            lua_setfield(L, 4, keyFirst);
            
            lua_pop(L, 1);
        } else {
            /* insert the value 
            printf("inserting into table %s key %s value %.5s\n", keyFirst, keySecond, (char *)value);*/
            lua_pushstring(L, keySecond);
            lua_pushstring(L, value);
            
            lua_settable(L, -3);
            
            lua_pop(L, 1);
        }
        
        free(keyFirst);
    } else {
        /* get env table value */
        lua_getfield(L, 4, key);
         if (!lua_istable(L, -1)) {
            lua_pushstring(L, (char *)value);
            lua_setfield(L, 4, key);
            
            lua_pop(L, 1);
         } else {
             printf("tried to set %s but it is a table\n", key);
             lua_pop(L, 1);
         }
    }
    return MAP_OK;
}

/*
Parse the template using the dictionary given
*/
char *parse_template(const char *in, const map_t dictionary) {
    return strdup(in);
}
