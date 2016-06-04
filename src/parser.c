#include "parser.h"

/*
get the next token
*/
char *parser_next_token(const char *in, const char *token) {
    const char *currChar = in;
    int i;
    int tokenLen = strlen(token);
    // loop through the entire string
    while (*currChar != '\0') {
        // check if currChar matches the first token
        if (*currChar == *token) {
            // loop through for the rest of the token lengths to see if they are equal
            for (i = 1; i < tokenLen; i++) {
                if (currChar[i] != token[i]) {
                    // the match failed at some point, keep looping through string then.
                    goto noMatch;
                }
            }
            // the loop finished successfully (ie, everything matches), so return
            return (char *)currChar;
        }
        // keep looping through string
        noMatch:
        currChar++;
    }
    // nothing was found that matched
    return NULL;
}

/*
Grabs the value between an input and an end bunch
ie, in=" test }}"" with end="}}" will return "test"

Returns NULL if nothing was found, otherwise will return a new null-terminated
string that is the value in between.

NOTUSED
char *parser_grab_value(const char *in, const char *end) {
    char *ending;
    int length;
    char *value;
    
    // get the ending, if it's not found, return NULL
    ending = parser_next_token(beginning, end);
    if (ending == NULL) return NULL;
    ending -= 1; // We want the last character before the ending, and 
                 // parser_next_token returns the start of the token
    
    length = ending - in;
    value = strndup(in, length);
    
    // check if there are any spaces at the start of at the end of the value
    if (*in == ' ' || *ending == ' ') {
        // trim the spaces and put the trimmed result into the value
        // need to free value since we just strdup'd it
        char *temp = strutil_trim_spaces(value);
        free(value);
        value = temp;
    }
    
    return value;
}*/

/*
Parse the template using the dictionary given
*/
char *parse_template(const char *in, const template_dictionary *dictionary) {
    const char *lastEnding = in;
    char *output = NULL, *newOutput, *startToken, *endToken,
         *tokenKey, *tokenValue;

    // loop through until we hit the end
    while (1) {
        // find the next occurence of "{{" - aka the start of a variable
        startToken = parser_next_token(lastEnding, "{{");
        if (startToken == NULL) goto done; // done parsing, no more variables
        
        printf("looping 1: %s\n", startToken);
        fflush(stdout);
        
        // find the next occurence of "}}" - aka the end of the variable
        endToken = parser_next_token(startToken, "}}");
        if (endToken == NULL) { // the endToken is null, should not happen!
            fprintf(stderr, "["KRED"ERR"RESET"] Template does not close variable! Check to make sure that all `{{` are terminated by `}}`.\n");
            return NULL;
        }
        
        printf("looping 2: %s\n", endToken);
        fflush(stdout);
        
        // append everything before the "{{"" into the result
        if (startToken == lastEnding) {
            // nothing to append, there is nothing in between
        } else if (output == NULL) {
            // first appending
            output = strndup(in, startToken - in);
        } else {
            newOutput = strutil_appendn_no_mutate(output, lastEnding,
                            startToken - lastEnding); // offset of the token
            free(output);
            output = newOutput;
        }
        
        printf("looping 3: %s\n", output);
        fflush(stdout);
        
        // extract what variable is getting spit out
        tokenKey = strndup(startToken+2, (endToken - startToken)-2);
        // 2 since "{{" is 2 long     1 since we need to go back 1 character
        if (*(startToken+2) == ' ' || *(endToken-1) == ' ') {
            // trim the spaces and put the trimmed result into the value
            // need to free value since we just strdup'd it
            char *temp = strutil_trim_spaces(tokenKey);
            free(tokenKey);
            tokenKey = temp;
        }
        
        printf("looping 4: (%ld) %s\n", (endToken - startToken)-2, tokenKey);
        fflush(stdout);
        
        // get that value from the dictionary
        // we do not ever mutate tokenValue, since it is a direct
        // reference from the dictionary
        tokenValue = td_fetch_val(dictionary, tokenKey);
        if (tokenValue == NULL) {
            tokenValue = "";
        }
        
        printf("looping 5: %s\n", tokenValue);
        fflush(stdout);
        
        // append that value in the result
        if (output == NULL) {
            output = strdup(tokenValue);
        } else {
            newOutput = strutil_append_no_mutate(output, tokenValue);
            free(output);
            output = newOutput;
        }
        
        printf("looping 6\n");
        fflush(stdout);
        
        // skip to the end of the "}}"
        lastEnding = endToken+2;
        
        printf("looping 7\n");
        fflush(stdout);
        
        // free the malloc'd strings
        free(tokenKey);
        
        printf("looping 8\n");
        fflush(stdout);
    }
    done:
    // check if there are any more non-variable characters to copy over
    if (*(lastEnding+1) != '\0') {
        newOutput = strutil_append_no_mutate(output, lastEnding);
        free(output);
        output = newOutput;
    }
    return output;
}