#ifndef _CMDS_INC
#define _CMDS_INC
#include "common.h"
#include "cfghelper.h"
#include "document.h"
#include "html.h"
#include <errno.h>
#include "parser.h"
#include "dictionary.h"

int newSrv(char *name);
int cleanNuDir(char *nuDir);
char *getNuDir(int argc, char**argv);
int buildNuDir(char *nuDir);
#endif