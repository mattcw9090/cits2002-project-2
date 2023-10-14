#include "mysync.h"
#include <stddef.h>
#include <stdlib.h>
#include <regex.h>

int deviceCount, numFilesDiscovered = 0;
int containerSize = 10;

char **deviceList = NULL;
struct File **allFileSet = NULL;

int numIpatterns = 0;
int numOpatterns = 0;

char **iGlobs = NULL;
char **oGlobs = NULL;


regex_t **iRegexList = NULL;
regex_t **oRegexList = NULL;

bool flag_all_files, flag_recursive, flag_ignore_pattern, flag_find_pattern, flag_identify, flag_permissions, flag_verbose = false;