#include "util.h"

/* utility to check if directory exists or not */
int dirExists(const char *name) {
    DIR* dir;
    dir = opendir(name);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return 1;
    } else if (errno == ENOENT) {
        /* Directory does not exist. */
        return 0;
    }
    return -1;
}

int makeNewDir(const char *dir) {
    int errCode;
    
    errCode = mkdir(dir, 0700);
    /* if worked, return 1 */
    if (errCode == 0) return 1;
    /* otherwise fallthrough to fail */
    return 0;
}

char *dirJoin(const char *a, const char *b) {
    /* join string helper function */
    int lenA;
    char *newString;
    
    lenA = strlen(a); /* for the null at the end*/
    
    /* check if there isn't already a trailing "/" */
    if (a[lenA-1] != '/') {
        /* allocate memory for them */
        newString = calloc(lenA+strlen(b)+2, sizeof(char));
        /* copy over first part */
        strcpy(newString, a);
        /* the slash */
        newString[lenA] = '/';
        /* then the new folder name */
        strcpy(&newString[lenA+1], b);
    } else {
        /* allocate memory for them */
        newString = calloc(lenA+strlen(b)+1, sizeof(char));
        /* copy over first part */
        strcpy(newString, a);
        /* then the new folder name */
        strcpy(&newString[lenA], b);
    }
    /* do not need to set null since we `calloc` */
    return newString;
}

/* get current directory */
int getCurrDir(char *location, int length) {
    if (getcwd(location, length) == NULL) {
        perror("["KRED"ERR"RESET"] ");
        fprintf(stderr, "["KRED"ERR"RESET"] Could not get current directory, failing!\n");
        return -1;
    }
    return 0;
}

int isNuDir(const char *dir) {
    FILE *fp;
    char *checkingFile;
    
    checkingFile = dirJoin(dir, NU_CONFIG_NAME);
    fp = fopen(checkingFile, "r");
    if (fp != NULL) {
        fclose(fp);
        free(checkingFile);
        return 1;
    } else {
        free(checkingFile);
        return 0;
    }
}

int delDir(const char *dirName) {
    DIR *folder;
    struct dirent *next_file;
    char *toDel;
    
    folder = opendir(dirName);
    if (folder == NULL) { /* already been deleted */ return 0; }
    
    while ((next_file = readdir(folder)) != NULL) {
        if (*(next_file->d_name) == '.') continue;
        toDel = dirJoin(dirName, next_file->d_name);
        if (dirExists(toDel)) delDir(toDel);
        if (remove(toDel) != 0) {
            fprintf(stderr, "["KRED"ERR"RESET"] Error clearing file %s: ", toDel);
            perror("");
            free(toDel);
            return 1;
        }
        free(toDel);
    }
    
    closedir(folder);
    return 0;
}

int loopThroughDir(const char *dirName, dirIterator iter) {
    DIR *folder;
    struct dirent *next_file;
    char *toRun;
    
    folder = opendir(dirName);
    if (folder == NULL) {perror("["KRED"ERR"RESET"] Error opening directory"); return 1;}
    
    while ((next_file = readdir(folder)) != NULL) {
        if (*(next_file->d_name) == '.') continue;
        toRun = dirJoin(dirName, next_file->d_name);
        if (iter(toRun) == -1) return -1;
        free(toRun);
    }
    
    closedir(folder);
    return 0;
}

const char *fileExtension(const char *in) {
    char *dot;
    
    dot = strrchr(in, '.');
    if(!dot || dot == in) return "";
    return dot + 1;
}

const char *fileName(const char *in) {
    return strrchr(in, '/');
}

#define isDigit(c) ((c) - '0' + 0U <= 9U)

char *getOutputFileName(const char *inFile, const char *nuDir, int *isSpecial) {
    int len;
    char *name, *outLocation, *temp, *temp2;
    
    /* make a copy of the input filename so we can mutate it */
    name = strdup(inFile);
    len = strlen(name);
    
    /* check if the filename matches the right format */
    if ((len > 15)
        && (isDigit(name[0]) && isDigit(name[1]) && isDigit(name[2]) && isDigit(name[3]) && name[4] == '-') /* year */
        && (isDigit(name[5]) && isDigit(name[6]) && name[7] == '-') /* month */
        && (isDigit(name[8]) && isDigit(name[9]) && name[10] == '-') /* day */) { /* this is a blog post */
        /* convert 2016-01-01-s.md to 2016/01/01/s */
        temp = (char *)fileExtension(name) - 1;
        *temp = '\0'; /* null terminate the string to remove the .md */
        name[4] = '/';
        name[7] = '/';
        name[10] = '/';
        temp = strutil_append_no_mutate(name, ".html"); /* add the .html */
        free(name);
        temp2 = dirJoin(nuDir, "posts/"); /* goes in posts dir */
        outLocation = dirJoin(temp2, temp);
        free(temp);
        free(temp2);
        *isSpecial = 0;
        return outLocation;
    } else { /* treat it as a special file */
        temp = (char *)fileExtension(name) - 1;
        *temp = '\0'; /* null terminate the string to remove the .md */
        temp = strutil_append_no_mutate(name, ".html"); /* add the .html */
        free(name);
        temp2 = dirJoin(nuDir, "special/"); /* goes in special dir */
        outLocation = dirJoin(temp2, temp);
        free(temp);
        free(temp2);
        *isSpecial = 1;
        return outLocation;
    }
}

void makeFnSafe(char *in) {
    char *fn;
    fn = strrchr(in, '/') + 1;
    while (*fn != '\0') {
        switch(*fn) {
            case '#':
            case ' ':
            case '/':
            case '!':
            case '\'':
            case '"':
            case '\\':
            case '?':
            *fn = '-';
            break;
        }
        fn++;
    }
}

char *dumpFile(const char *filename) {
    /* http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c */
    char *buffer = 0;
    long length;
    FILE *f;
    
    f = fopen (filename, "rb");
    
    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell (f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length+1);
        if (buffer) {
            fread (buffer, 1, length, f);
            buffer[length] = '\0';
        }
        fclose (f);
    }
    
    if (buffer) {
        return buffer;
    }
    return NULL;
}

int writeFile(const char *filename, const char *toWrite) {
    FILE *fp;
    int throwErr;
    throwErr = 0;
retry:
    fp = fopen(filename, "w+");
    
    if (fp != NULL) {
        fputs(toWrite, fp);
        fclose(fp);
        throwErr = 0;
        return 0;
    } else {
        if (throwErr) {
            fprintf(stderr, "["KRED"ERR"RESET"] Failed to write to output file %s: ", filename);
            perror("");
            return -1;
        } else {
            /* try creating the dir instead */
            createDirs(filename);
            throwErr = 1;
            goto retry;
        }
    }
}

char *parseMD(const char *filename) {
    FILE *in;
    hoedown_buffer *ib, *ob;
	hoedown_document *document;
	hoedown_renderer *renderer;
	char *ret = NULL;
	unsigned int removeCount;
	unsigned int extensions = HOEDOWN_EXT_NO_INTRA_EMPHASIS | HOEDOWN_HTML_HARD_WRAP | HOEDOWN_EXT_TABLES | HOEDOWN_EXT_UNDERLINE | HOEDOWN_EXT_HIGHLIGHT | HOEDOWN_EXT_SUPERSCRIPT | HOEDOWN_EXT_STRIKETHROUGH | HOEDOWN_EXT_FENCED_CODE | HOEDOWN_EXT_AUTOLINK;
	
    /* input file */
	in = fopen(filename, "r");
	if (in == NULL) {
		fprintf(stderr, "["KRED"ERR"RESET"] Failed to open input file %s: \n", filename);
		perror("");
		return NULL;
	}
	
	/* Use hoedown to parse Markdown
	   If you don't understand this code, check out https://github.com/hoedown/hoedown/wiki/Getting-Started */
	/* create the buffers */
	ib = hoedown_buffer_new(16);
	hoedown_buffer_putf(ib, in);
	renderer = hoedown_html_renderer_new(0, 0);
	
	/* Perform Markdown rendering */
	ob = hoedown_buffer_new(16);
	document = hoedown_document_new(renderer, extensions, 16);

	hoedown_document_render(document, ob, ib->data, ib->size);

	hoedown_buffer_free(ib);
	hoedown_document_free(document);
	hoedown_html_renderer_free(renderer);
	
	fclose(in);
	
	removeCount = strutil_remove_unicode(ob->data, ob->size, &ret);
	if (removeCount > 0) {
	    printf("["KYEL"WARN"RESET"] Unicode characters were found in the file `%s`. Unfortunately, nu v"NU_VERSION" currently does NOT support unicode, so those characters were removed.\n", filename);
	}
	return ret;
}

int createDirs(const char *filename) {
    char *temp, *curr;
    struct stat st = {0};
    
    temp = strdup(filename);
    curr = strrchr(temp, '/');
    *curr = '\0';
    
    if (stat(temp, &st) == -1) {
        createDirs(temp);
        mkdir(temp, 0700);
    }
    
    free(temp);
    return 0;
}
