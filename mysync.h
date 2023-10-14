#include <stdbool.h>
#include <time.h>
#include <regex.h>

/**
 * File:
 * Represents a File object stored in allFileSet
 * 
 * This structure holds information related to a file, including its name (location),
 * modification time, and the device or base path where it's located.
 */
struct File {
    char *identifier;  // String corresponding to the file's name (location). Example format: "/home/test.txt"
    time_t modtime;    // Timestamp of the file’s last modification time
    char *device;      // String representing the device or base path
};



/**
 * allFileSet:
 * an array of pointers to File objects
 * 
 * This data structure is used to store all UNIQUE File objects 
 * discovered during the initial scan of all input devices
 */
extern struct File **allFileSet;




/**
 * deviceList:
 * an array of strings where each string represents a device or base path
 * 
 * Used to store and access devices or base paths
 */
extern char **deviceList;

extern char **iGlobs;                       // array of glob strings for -i argument
extern char **oGlobs;                       // array of glob strings for -o argument

extern regex_t **iRegexList;                  // array of regex structures containing compiled pattern for -i argument
extern regex_t **oRegexList;                  // array of regex structures containing compiled pattern for -o argument

extern int numIpatterns;                           // number of -i patterns recorded
extern int numOpatterns;                           // number of -o patterns recorded


extern int deviceCount;                     // number of devices recorded
extern int numFilesDiscovered;              // total number of files within allFileSet
extern int containerSize;                   // size of allFileSet which is dynamically resizeable


extern bool flag_all_files;                 // all hidden files to be synced
extern bool flag_recursive;                 // all nested files to be synced
extern bool flag_ignore_pattern;            // all files matching the mattern to be ignored
extern bool flag_find_pattern;              // all files matching the mattern to be synced
extern bool flag_identify;                  // any files to be copied are identified, but not synchronised
extern bool flag_permissions;               // copy same mod time and same permissions as the old file
extern bool flag_verbose;                   // verbose output




// Functions from criticalFunctions.c
void read_directory(char *, char *);
void update_allFileSet(struct File *);
bool does_file_exist(char *, struct File *);
void copy_file_contents(char *, char *);
void synchronise(char *);
void construct_path(char *);
bool does_directory_exist(char *);
void copy_attr(char *, char *);


// Functions from prints.c (debugging and printing functions)
void print_file(struct File *);
void print_all_files();
void print_np_failure(void *);
void print_devices(); 


// Functions from strings.c (pathway string manipulation functions)
char* get_parent_directory(char *);
char *get_full_path(char *, char *);
char *get_identifier(char *, char *);


// Functions from regex.c (regex functions)
char *glob2regex(char *);
void compile_globs(char **, regex_t **, int);