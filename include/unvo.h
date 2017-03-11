#ifndef _UNVO_PARSER_INC
#define _UNVO_PARSER_INC
#define LUA_USE_APICHECK

#include "libstring.h"
#include "common.h"
#include "goatee_gen.h"
#include "goatee_run.h"
#include "util.h"
#include "goatee_logger.h"
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int hm_transfer_iter_func(void *luaRef, const char *key, void *value);
char *parse_template_withmap(const char *in, lua_State *L);
char *parse_template(const char *in, const map_t dictionary, lua_State *L);
#endif
