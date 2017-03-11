#include "unvo.h"

static goatee_logger *gl = NULL;
static string unvo_handle_include(string all, string in);

static struct handlerInfo myModifiers[] = {
    {'#', '#', &goatee_gen_handle_comment},
    {'%', '%', &goatee_gen_handle_exec},
    {'{', '}', &goatee_gen_handle_var},
    {'+', '+', &unvo_handle_include},
    {0,0, NULL}
};

static string unvo_handle_include(string all, string in) {
    string fileIn, tmp, tmp3;
    char *tmp2, *tmp4, *tmp5;
    
    tmp2 = goatee_trim_spaces(in);
    tmp4 = dirJoin(globNuDir, "scripts/");
    tmp5 = dirJoin(tmp4, tmp2);
    free(tmp2);
    free(tmp4);
    fileIn = goatee_dump_file(tmp5);
    
    if (fileIn == NULL) {
        tmp3 = string_append("Failed to find file ", tmp5);
        gl->log(gl, GLL_WARN, tmp3);
        string_free(tmp3);
    
        free(tmp5);
        return all;
    }
    
    free(tmp5);
    
    tmp = goatee_gen_noHeader(fileIn, myModifiers, 0);
    string_free(fileIn);
    
    return string_append(all, string_temporary(
        string_appendv(3, "\n", string_temporary(tmp), "\n")
    ));
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
#endif

int hm_transfer_iter_func(void *luaRef, const char *key, void *value) {
    lua_State *L = (lua_State *)luaRef;
    char *keyFirst, *keySecond, *dotLocation;
    
    /* expecting luaRef to be on top of stack */
    
    dotLocation = strchr(key, '.');
    /* handle cases where we need a table */
    if (dotLocation != NULL) {
        keyFirst = strdup(key);
        keySecond = keyFirst + (dotLocation-key);
        
        /* set the dot to null */
        *(keySecond++) = '\0';
        
        /* get env table value */
        lua_getfield(L, -1, keyFirst);

        if (!lua_istable(L, -1)) {
            /* need to create the table 
            printf("creating table `%s` to insert key `%s`\n", keyFirst, key); */
            lua_newtable(L);

            /* insert the value 
            printf("inserting into table `%s` key `%s` value `%.5s`\n", keyFirst, keySecond, (char *)value);*/
            lua_pushstring(L, keySecond);
            lua_pushstring(L, value);
            
            lua_settable(L, -3);

            lua_setfield(L, -3, keyFirst);

            lua_pop(L, 1);
        } else {
            /* insert the value 
            printf("inserting into table %s key %s value %.5s\n", keyFirst, keySecond, (char *)value); */
            lua_pushstring(L, keySecond);
            lua_pushstring(L, value);

            lua_settable(L, -3);
            lua_pop(L, 1);
        }
        
        free(keyFirst);
    } else {
        /* get env table value */
        lua_getfield(L, -1, key);

         if (!lua_istable(L, -1)) {
            lua_pushstring(L, (char *)value);
            /*printf("all ok here!");stackDump(L);fflush(stdout);*/
            lua_setfield(L, -3, key);
            
            lua_pop(L, 1);
         } else {
             printf("tried to set %s but it is a table\n", key);

             lua_pop(L, 1);
         }
    }
    return MAP_OK;
}

#if 0
static void table_dump(lua_State *L) {
    int t = 1;
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

char *parse_template_withmap(const char *in, lua_State *L) {
    /* expects that table env will be on top */
    string out, outFinal, tmp;
    
    /* create logger */
    gl = goatee_logger_new(GLL_WARN);
    
    /* convert to string */
    tmp = string_mknew(in);
    
    /* generate lua code to run */
    out = goatee_gen(tmp, myModifiers, gl);
    if (!out) {
        printf("%s", gl->messages);
        return NULL;
    }
    string_free(tmp);
    
    /* run the lua code */
    outFinal = goatee_run(L, out, gl);
    if (!outFinal) {
        printf("%s", gl->messages);
        printf("output:\n%s", out);
        return NULL;
    }
    
    printf("%s", gl->messages);
    
    /* cleanup */
    goatee_logger_destroy(gl);
    string_free(out);
    return outFinal;
}

/*
Parse the template using the dictionary given
*/
char *parse_template(const char *in, const map_t dictionary, lua_State *L) {
    string out;
    
    goatee_setup_basic_table(L);
    hashmap_iterate(dictionary, &hm_transfer_iter_func, L);

    out = parse_template_withmap(in, L);
    
    lua_pop(L, 1);
    return out;
}
