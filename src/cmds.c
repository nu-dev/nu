#include "cmds.h"

static char newsrv_buf[BUF_SIZE];

static const char *defaultConfig_contents =
"# nu default config\n"
"# for help, please visit https://github.com/nu-dev/nu/wiki/Getting-Started\n"
"THEME = \"basic\"\n"
"IGNORENEWERPOST = \"1\"\n";

int newSrv(char *name) {
    int nameLen = strlen(name);
    int cwdLen = 0;
    int i;
    char *dirsToMake[6] = {"", "posts", "raw", "resources", "special", "theme"};
    int dirsToMakeCount = 6;
    char *configName = "config.kg";
    int configNameLength = strlen(configName);
    char *newDirName;
    FILE *fp;
    
    if(getCurrDir(newsrv_buf, BUF_SIZE - nameLen - configNameLength) == -1) {
        return -1;
    }
    cwdLen = strlen(newsrv_buf);

    // copy over the project name
    newsrv_buf[cwdLen] = '/';
    for (i = 0; i <= nameLen; i++)
        newsrv_buf[cwdLen+i+1] = name[i];
    
    // make sure directory doesn't already exist
    if (dirExists(newsrv_buf)) {
        fprintf(stderr, "["KRED"ERR"RESET"] Directory %s already exists!\n", newsrv_buf);
        return -1;
    }
    
    for (i = 0; i < dirsToMakeCount; i++) { // loop through all the directories to make
        printf("["KBLU"INFO"RESET"] Making directory %s/%s!\n", newsrv_buf, dirsToMake[i]);
        newDirName = dirJoin(newsrv_buf, dirsToMake[i]);
        if (makeNewDir(newDirName) == 0) { // make directory itself
            fprintf(stderr, "["KRED"ERR"RESET"] Failed  to make directory %s/%s! (Error #%d)\n", newsrv_buf, dirsToMake[i], errno);
            free(newDirName);
            return -1;
        }
        free(newDirName);
    }
    
    // copy over config name
    newsrv_buf[cwdLen+nameLen+1] = '/';
    for (i = 0; i < configNameLength; i++)
        newsrv_buf[cwdLen+nameLen+i+2] = configName[i];

    // create the file
    fp = fopen(newsrv_buf, "w");
    if (fp == NULL) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to open file %s! (Error #%d)\n", newsrv_buf, errno);
        return -1;
    }
    
    // write into it
    fputs(defaultConfig_contents, fp);
    fclose(fp);
    
    return 0;
}

int cleanNuDir(char *nuDir) {
    char *removingDir;
    int hasErr = 0;
    removingDir = dirJoin(nuDir, "posts");
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
    }
    free(removingDir);
    removingDir = dirJoin(nuDir, "special");
    if (delDir(removingDir) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to clear directory %s! Check if you have permissions to delete.\n", removingDir);
        hasErr = 1;
    }
    free(removingDir);
    return hasErr;
}

char *getNuDir(int argc, char** argv) {
    char *buf = calloc(BUF_SIZE, sizeof(char));
    char *nuDir;
    if (argc == 2) { // no other arguments passed, assume user means current directory is nudir
        if (getCurrDir(buf, BUF_SIZE) != 0) return NULL;
        nuDir = buf;
    } else if (argc == 3) { // specified a directory to use as nudir
        if (getCurrDir(buf, BUF_SIZE) != 0) return NULL;
        nuDir = dirJoin(buf, argv[2]);
        free(buf);
    } else {
        fprintf(stderr, "["KRED"ERR"RESET"] Invalid number of arguments passed. For help, use `%s help clean`\n", *argv);
        free(buf);
        return NULL;
    }
    if (!isNuDir(nuDir)) goto notnudir;
    return nuDir;
notnudir:
    fprintf(stderr, "["KRED"ERR"RESET"] The specified directory %s is not a valid nu directory. Please check that the file `config.cfg` exists and try again.\n", nuDir);
    free(nuDir);
    return NULL;
}

static char *globNuDir;
static template_dictionary *template_dic;

int builderHelper(char *inFile) {
    FILE *in;
    FILE *out;
    char *outFile;
    char *final;
    hoedown_buffer *ib, *ob;
	hoedown_document *document;
	template_dictionary *post_dic;
	unsigned int extensions = HOEDOWN_EXT_NO_INTRA_EMPHASIS | HOEDOWN_HTML_HARD_WRAP | HOEDOWN_EXT_TABLES | HOEDOWN_EXT_UNDERLINE | HOEDOWN_EXT_HIGHLIGHT | HOEDOWN_EXT_SUPERSCRIPT | HOEDOWN_EXT_STRIKETHROUGH | HOEDOWN_EXT_FENCED_CODE | HOEDOWN_EXT_AUTOLINK;

    // get the output filename
    outFile = getOutputFileName(inFile, globNuDir);
    
    if (!(fileTimeDelta(inFile, outFile) > 0)) { // the markdown file is older than the compiled file
        printf("["KBLU"ERR"RESET"] Skipping file %s (input file is older than compiled file)\n", inFile);
        return 0;
    } else {
        printf("["KBLU"ERR"RESET"] Building file %s to %s\n", inFile, outFile);
    }
    
    // input file
	in = fopen(inFile, "r");
	if (in == NULL) {
		fprintf(stderr, "["KRED"ERR"RESET"] Failed to open input file %s: ", inFile);
		perror("");
		return -1;
	}
	
	// output file
	out = fopen(outFile, "w+");
	if (out == NULL) {
		fprintf(stderr, "["KRED"ERR"RESET"] Failed to open output file %s: ", outFile);
		perror("");
		return -1;
	}
	
	/* If you don't understand this code, check out https://github.com/hoedown/hoedown/wiki/Getting-Started */
	// create the buffers
	ib = hoedown_buffer_new(16);
	hoedown_buffer_putf(ib, in);
	hoedown_renderer *renderer = hoedown_html_renderer_new(0, 0);
	
	/* Perform Markdown rendering */
	ob = hoedown_buffer_new(16);
	document = hoedown_document_new(renderer, extensions, 16);

	hoedown_document_render(document, ob, ib->data, ib->size);

	hoedown_buffer_free(ib);
	hoedown_document_free(document);
	hoedown_html_renderer_free(renderer);
	
	fclose(in);
	fclose(out);
	
	// create the post dictionary
	post_dic = td_new();
	td_put(post_dic, "post.content", ob->data);
	td_put(post_dic, "post.date", );
	td_put(post_dic, "post.name", );
	
	final = parse_template(, td);
	
	fprintf(out, "%s", final);
	free(outFile);
	return 0;
}

int buildNuDir(char *nuDir) {
    char *buildingDir;
    buildingDir = dirJoin(nuDir, "raw");
    globNuDir = nuDir;
    if (loopThroughDir(buildingDir, &builderHelper) != 0) {
        fprintf(stderr, "["KRED"ERR"RESET"] Failed to build! Check if you have permissions to create files in `%s/posts` or `%s/special` and try again.\n", nuDir, nuDir);
        free(buildingDir);
        return -1;
    }
    free(buildingDir);
    return 0;
}