#include "cfghelper.h"

int parse_config(const char *in, const char *prefix, template_dictionary *dictionary) {
    char *start, *currkey, *temp, *currvalue, *ending;
    // loop through until we hit the end
    const char *last = in;
    while (1) {
        // find next occurence of '=' - aka the start of a declaration - if not found, we're done here!
        start = strutil_next_token(last, "=");
        if (start == NULL) goto done;
        
        // everything from the last char to this equal sign is the left side - unless it's '#'
        if (*last == '#') {
            last = strutil_next_token(last, "\n");
            if (last == NULL) goto done;
            else last++;
            continue;
        }
        currkey = strndup(last, start - last);
        
        // strip spaces from it
        temp = strutil_trim_spaces(currkey);
        free(currkey);
        currkey = temp;
        
        // add prefix to it (prefix should already be including the ".")
        temp = strutil_append_no_mutate(prefix, currkey);
        free(currkey);
        currkey = temp;
        
        temp = start;
        
        // keep reading until we hit a " or a < - if not found, throw error
        while (*temp != '"' && *temp != '<'  && *in != '\0'  && *in != '\n') {
            temp++;
        }
        
        if (*temp == '\0' || *temp == '\n') {
            fprintf(stderr, "["KRED"ERR"RESET"] The config key `%s` does not have a value!\n", currkey);
            free(currkey);
            return 1;
        } else if (*temp == '"') { // if it's a ":
            // save that location into a variable
            currvalue = temp + 1; // skip past the "
            
            // read until you hit " - if not found, throw error
            temp = strutil_next_token(currvalue, "\"");
            
            if (temp == NULL) {
                fprintf(stderr, "["KRED"ERR"RESET"] The config key `%s` does not have a value!\n", currkey);
                free(currkey);
                return 1;
            }
            
            // from the previously saved location to the " is the value
            currvalue = strndup(currvalue, temp - currvalue);
            
            // save it in the dictionary
            td_put_val(dictionary, currkey, currvalue);
            free(currkey);
            free(currvalue);
            
            // save the location to the ending
            ending = temp+2;
        } else if (*temp == '<') { // else (aka it's a <)
            char *lookupStr;
            
            // make sure next 2 chars are < and < (and not anything else - if they are, throw error)
            if (!(*(temp+1) == '<' && *(temp+2) == '<')) {
                fprintf(stderr, "["KRED"ERR"RESET"] The config key `%s` does not have a value!\n", currkey);
                free(currkey);
                return 1;
            }
            temp += 3;
            // read next 3 non-null characters - if they are null, throw error
            if (*(temp) != '\0' && *(temp+1) != '\0' && *(temp+2) != '\0' && *(temp) != '\n' && *(temp+1) != '\n' && *(temp+2) != '\n') {
                // save the end of those 3 characters into a variable
                lookupStr = strndup(temp, 3);
                currvalue = temp + 4; // skip past all the characters (including newline)
            } else {
                fprintf(stderr, "["KRED"ERR"RESET"] The config key `%s` has an invalid value. For help, please visit the wiki (use the flag `help` for a link).\n", currkey);
                free(currkey);
                return 1;
            }
            
            // use those 3 characters to look for the end - if not found, throw error
            temp = strutil_next_token(currvalue, lookupStr);
            if (temp == NULL) {
                fprintf(stderr, "["KRED"ERR"RESET"] The config key `%s` has an invalid value. For help, please visit the wiki (use the flag `help` for a link).\n", currkey);
                free(currkey);
                free(lookupStr);
                return 1;
            }
            
            // from the variable stored to the recently found end is the value
            currvalue = strndup(currvalue, temp - currvalue);
            free(lookupStr);
            
            // save that to the template dictionary
            td_put_val(dictionary, currkey, currvalue);
            free(currkey);
            free(currvalue);
            
            // ending is set to 4 characters past the end (include newline)
            ending = temp + 4;
            
        }
        // jump to the ending
        last = ending;
        if (last == NULL || *last == '\0')  goto done;
    }
    done:
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        FILE *f;
        long length;
        char *buffer;

        f = fopen(argv[1], "r");
        if (f) {
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            buffer = malloc (length);
            if (buffer) {
                fread (buffer, 1, length, f);
            }
            fclose (f);
        }
        
        if (buffer) {
            template_dictionary *td = td_new();
            template_dictionary_entry *temp;
            parse_config(buffer, "test.", td);
            temp = td->entryList;
            while (temp != NULL) {
                printf("entry_k=%s, entry_v=%s\n", temp->key, temp->value);
                temp = temp->next;
            }
            td_clean(td);
            free(buffer);
        }

    } else {
        printf("Invalid # of arguments!");
    }
    return 1;
}