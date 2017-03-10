#ifndef _UNVO_PARSER_INC
#define _UNVO_PARSER_INC

#include "common.h"
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

char *parse_template(const char *in, const map_t dictionary);
#endif
