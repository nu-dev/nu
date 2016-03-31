#ifndef _CFGHELPER_INC
#define _CFGHELPER_INC
typedef struct cfg_loadedinfo {
	char *theme;
	char *;
	char *srvdir;
	int cache, cachetime;
} nucfg;
void loadcfg(char *filename, nucfg *file);
void cleanupcfg(mucfg *file);
#endif