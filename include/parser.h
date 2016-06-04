#ifndef _UNVO_PARSER_INC
#define _UNVO_PARSER_INC
#include "libnucommon.h"
char *parser_next_token(const char *in, const char *token);
char *parse_template(const char *in, const template_dictionary *dictionary);
#endif