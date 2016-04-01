#ifndef _UTILHEADER_INC
#define _UTILHEADER_INC
#include "common.h"
int dirExists(char *name);
int makeNewDir(char *dir);
char *dirJoin(char *a, char *b);
int getCurrDir(char *location, int length);
int isNuDir(char *dir);
#endif