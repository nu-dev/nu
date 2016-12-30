#ifndef _LUAT_PARSER_INC
#define _LUAT_PARSER_INC

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "luat_file.h"


char *luat_parse(const char *str, const map_t dictionary);
#endif
