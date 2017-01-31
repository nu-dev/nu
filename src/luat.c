#include "luat.h"

#if false
static void stackDump(lua_State *L) {
    int i;
    int top = lua_gettop(L);
    for (i = top; i >= 1; i--) {  /* repeat for each level */
      int t = lua_type(L, i);
      printf("%d: ", i);
      switch (t) {
        case LUA_TSTRING:  /* strings */
          printf("string");
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
#endif

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


/* code portions from https://john.nachtimwald.com/2014/08/06/using-lua-as-a-templating-engine/
 * and from https://john.nachtimwald.com/2014/07/26/calling-lua-from-c/ */

static struct {
    char *name;
} globals[] = {
    { "pairs"  },
    { "ipairs" },
    { "type"   },
    { "table"  },
    { "string" },
    { "math"   },
    { NULL     }
};

char *luat_parse(const char *str, const map_t dictionary) {
    lua_State *L;
    size_t          i;
    char *output;
    
    L = luaL_newstate();
    luaL_openlibs(L);

    /* Load the template module which we will run our template through. */
    if (luaL_loadbuffer(L, (const char *)objs_luat_file_luac, objs_luat_file_luac_len, "luat.lua")
        || lua_pcall(L, 0, LUA_MULTRET, 0)) {
        printf("Could not load the template engine: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return NULL;
    }

    /* Put the function we're going to run on the stack. */
    lua_getfield(L, -1, "compile");
    /* Put the template string on the stack that we will use. */
    lua_pushstring(L, str);
    
    printf("%s\n", lua_tostring(L, -1));

    /* Push a table which will be our sandboxed env onto the stack. */
    lua_newtable(L);

    /* Push each of the system libs we want available into the table */
    for (i = 0; globals[i].name != NULL; i++) {
        lua_getglobal(L, globals[i].name);
        lua_setfield(L, -2, globals[i].name);
    }
    /* os.date needs to be pushed a bit differently since we only want
     * to push parts of it. */
    lua_getglobal(L, "os");
    lua_getfield(L, -1, "date");
    /* Set os.date to date in our table. */
    lua_setfield(L, -3, "date");
    /* Remove os from the stack. */
    lua_pop(L, 1);
    
    /* Push everything in the hashmap on to the stack */
    hashmap_iterate(dictionary, &hm_transfer_iter_func, L);
    
    /* Right now we have the stack as follows:
     * -1 env table
     * -2 template filename
     * -3 template function
     * -4 template Module 
     */
    if (lua_pcall(L, 2, LUA_MULTRET, 0) != 0) {
        printf("Error in template: `%s`\n", str);
        printf("%s\n", lua_tostring(L, -1));
        
        lua_close(L);
        exit(123213);
        return NULL;
    }
    
    output = strdup(lua_tostring(L, -1));

    /* Print the template (or error message) */
    lua_close(L);
    return output;
}
