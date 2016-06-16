#include "unvo.h"

/*
Parse the template using the dictionary given
*/
char *parse_template(const char *in, const template_dictionary *dictionary) {
    const char *lastEnding = in;
    char *output = NULL, *newOutput, *startToken, *endToken,
         *tokenKey;
    const char *tokenValue;

    /* loop through until we hit the end */
    while (1) {
        /* find the next occurence of "{{" - aka the start of a variable */
        startToken = strutil_next_token(lastEnding, "{{");
        if (startToken == NULL) goto done; /* done parsing, no more variables */
        
        /* find the next occurence of "}}" - aka the end of the variable */
        endToken = strutil_next_token(startToken, "}}");
        if (endToken == NULL) { /* the endToken is null, should not happen! */
            fprintf(stderr, "["KRED"ERR"RESET"] Template does not close variable! Check to make sure that all `{{` are terminated by `}}`.\n");
            return NULL;
        }
        
        /* append everything before the "{{"" into the result */
        if (startToken == lastEnding) {
            /* nothing to append, there is nothing in between */
        } else if (output == NULL) {
            /* first appending */
            output = strndup(in, startToken - in);
        } else {
            newOutput = strutil_appendn_no_mutate(output, lastEnding,
                            startToken - lastEnding); /* offset of the token */
            free(output);
            output = newOutput;
        }
        
        /* extract what variable is getting spit out */
        tokenKey = strndup(startToken+2, (endToken - startToken)-2);
        /* 2 since "{{" is 2 long     1 since we need to go back 1 character */
        if (*(startToken+2) == ' ' || *(endToken-1) == ' ') {
            /* trim the spaces and put the trimmed result into the value */
            /* need to free value since we just strdup'd it */
            char *temp = strutil_trim_spaces(tokenKey);
            free(tokenKey);
            tokenKey = temp;
        }
        
        /* get that value from the dictionary */
        /* we do not ever mutate tokenValue, since it is a direct */
        /* reference from the dictionary */
        tokenValue = td_fetch_val(dictionary, tokenKey);
        if (tokenValue == NULL) {
            tokenValue = "";
        }
        
        /* append that value in the result */
        if (output == NULL) {
            output = strdup(tokenValue);
        } else {
            newOutput = strutil_append_no_mutate(output, tokenValue);
            free(output);
            output = newOutput;
        }
        
        /* skip to the end of the "}}" */
        lastEnding = endToken+2;

        /* free the malloc'd strings */
        free(tokenKey);
    }
    done:
    /* check if there are any more non-variable characters to copy over */
    if (*(lastEnding+1) != '\0') {
        newOutput = strutil_append_no_mutate(output, lastEnding);
        free(output);
        output = newOutput;
    }
    return output;
}
