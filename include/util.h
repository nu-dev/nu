#ifndef _UTILHEADER_INC
#define _UTILHEADER_INC
#include "common.h"
#include "document.h"
#include "html.h"
#include "kg.h"
#include "post.h"
#include <string.h>
#include <errno.h>

int dirExists(const char *name);
int makeNewDir(const char *dir);
char *dirJoin(const char *a, const char *b);
int getCurrDir(char *location, int length);
int isNuDir(const char *dir);
int delDir(const char *dirName);

/* function pointer def */
typedef int (*dirIterator)(const char *inFile);

int loopThroughDir(const char *dirName, dirIterator iter);
const char *fileExtension(const char *in);
const char *fileName(const char *in);
char *getOutputFileName(const char *inFile, const char *nuDir, int *isSpecial);
char *dumpFile(const char *filename);
int writeFile(const char *filename, const char *toWrite);
void makeFnSafe(char *in);
void parseFile(const char *filename, post *p);
char *parseMD(const char *in, const char *filename);
int createDirs(const char *filename);
#endif
