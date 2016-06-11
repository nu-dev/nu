#ifndef _UTILHEADER_INC
#define _UTILHEADER_INC
#include "common.h"
#include <errno.h>

int dirExists(const char *name);
int makeNewDir(const char *dir);
char *dirJoin(const char *a, const char *b);
int getCurrDir(char *location, int length);
int isNuDir(const char *dir);
int delDir(const char *dirName);

// function pointer def
typedef int (*dirIterator)(char *dirName);

int loopThroughDir(const char *dirName, dirIterator iter);
int fileTimeDelta(const char *nameOne, const char *nameTwo);
time_t getFileLastModifiedTime(const char *name);
const char *fileExtension(const char *in);
const char *fileName(const char *in);
char *getOutputFileName(const char *inFile, const char *nuDir);
char *dumpFile(const char *filename);
#endif