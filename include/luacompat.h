#ifndef _LUACOMPAT_INC
#define _LUACOMPAT_INC
#include "common.h"
#include "util.h"
#include "libstring.h"
#include "goatee_run.h"
#include "goatee_gen.h"

static int template_require(lua_State *L);
static int template_include(lua_State *L);

#endif
