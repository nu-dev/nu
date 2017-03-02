#include "build.h"

int builderHelper(const char *inFile) {
    static post *temp;
    static const char *ext;

    ext = fileExtension(inFile);
    if (strcmp(ext, "md") != 0 && strcmp(ext, "markdown") != 0) {
        printf("["KYEL"WARN"RESET"] Skipping file %s - file extension is not `md` or `markdown`.\n", inFile);
        return 0;
    }

    temp = post_create(inFile);
    if (temp == NULL) {
        return -1;
    }
    pl_add_post(pl, temp);
	return 0;
}

void build_post(char *inFileName, char *outFileName) {
    
}

void build_special(char *inFileName, char *outFileName) {
    
}

void build_index(char *inFileName, ) {
    
}