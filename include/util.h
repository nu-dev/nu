#ifndef _UTILHEADER_INC
#define _UTILHEADER_INC
#include "common.h"
#include <errno.h>

int dirExists(char *name);
int makeNewDir(char *dir);
char *dirJoin(char *a, char *b);
int getCurrDir(char *location, int length);
int isNuDir(char *dir);
int delDir(char *dirName);

// function pointer def
typedef int (*dirIterator)(char *dirName);

int loopThroughDir(char *dirName, dirIterator iter);
int fileTimeDelta(char *nameOne, char *nameTwo);

char *getOutputFileName(char *inFile, char *nuDir);
#endif