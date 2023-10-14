#include "mysync.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <regex.h>
#include <utime.h>

//  converts a glob pattern into regex pattern
//	dynamically allocated output
char *glob2regex(char *glob){
    char *re = NULL;
    if (glob != NULL) {
        re = calloc(strlen(glob) * 2 + 4, sizeof(char));
        if (re == NULL) {
            return NULL;
        }
        char *r = re;
        *r++ = '^';
        while (*glob != '\0') {
            switch (*glob) {
                case '.':
                case '\\':
                case '$':
                    *r++ = '\\';
                    *r++ = *glob++;
                    break;
                case '*':
                    *r++ = '.';
                    *r++ = *glob++;
                    break;
                case '?':
                    *r++ = '.';
                    glob++;
                    break;
                case '/':
                    free(re);
                    re = NULL;
                    break;
                default:
                    *r++ = *glob++;
                    break;
            }
        }
        if (re) {
            *r++ = '$';
            *r = '\0';
        }
    }
    return re;
}


/**
 * void compile_globs(char **globs, regex_t **regexList, int num);
 * 
 * Compiles array of glob strings into array of regex structure
 * 
 * globs: An array of glob strings
 * regexList: An array of pointers to regex structures
 */
void compile_globs(char **globs, regex_t **regexList, int num){
    // iterates through each glob string and regex structure
    for (int i=0; i<num; i++){
        // allcoation of memory to regex structure
        regex_t *regexStruct = (regex_t *)malloc(sizeof(regex_t));

        // conversion of glob string to regex string
        char *regexStr = glob2regex(globs[i]);
        print_np_failure(regexStr);

        // compile regex string into regex structure
        regcomp(regexStruct, regexStr, 0);

        // putting regex structure into the array
        regexList[i] = regexStruct;

        free(regexStr);
    }
}

