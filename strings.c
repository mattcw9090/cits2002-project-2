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

/**
 * char* get_parent_directory(char *fullPath);
 * 
 * Given an absolute path, return the full path of its parent directory
 *
 * fullPath: A string representing a full path
 * 
 * return: full path of parent directory is returned. This string has memory dynamically allocated 
 * and has to be freed after usage
 */
char* get_parent_directory(char *fullPath){

    // locate last occurrence of '/'
    char *last_slash = strrchr(fullPath, '/');

    // allocate memory for parent directory string
    char *parent = (char *)malloc((last_slash - fullPath + 1) * sizeof(char));
    print_np_failure(parent);

    // copy string over over
    strncpy(parent, fullPath, last_slash - fullPath);
    parent[last_slash - fullPath] = '\0';
    

    return parent;
}



/**
 * char *get_full_path(char *devicePath, char *identifier);
 * 
 * Constructs and returns the full path of a file.
 *
 * This function concatenates the given device path, and entry name
 * to return the full path of a file. Memory for the full path string is
 * dynamically allocated; thus, it's the caller’s responsibility to free this memory
 * when it is no longer needed
 *
 * devicePath: A string representing the device or base path
 * identifier: A string representing the name (location) of the file
 * 
 * return: Returns a dynamically allocated string containing the full path of the file
 */
char *get_full_path(char *devicePath, char *identifier){
    
    // Add 2 for the null terminator and possible extra '/'
    size_t total_length = strlen(devicePath) + strlen(identifier) + 2; 


    // identifier to be formatted into a way such that concatenation produces correct format
    char *modified_identifier = malloc((strlen(identifier) + 2) * sizeof(char)); // Add 2 for possible '/' and null terminator
    print_np_failure(modified_identifier);

    // Check if identifier is just '/'
    if (strcmp(identifier, "/") == 0) {
        modified_identifier[0] = '\0'; // Set modified_identifier to an empty string
    } 
    else if (identifier[0] == '/') {
        // If identifier already starts with '/', copy it as is into modified_identifier
        strcpy(modified_identifier, identifier);
    } 
    else {
        // If identifier doesn't start with '/', prepend '/' to it
        sprintf(modified_identifier, "/%s", identifier);
    }
    

    // Allocate memory for the concatenated string.
    char* concatenated_string = (char*)malloc(total_length * sizeof(char));
    print_np_failure(concatenated_string);
    
    
    // construct the full path string
    strcpy(concatenated_string, devicePath);
    strcat(concatenated_string, modified_identifier);


    free(modified_identifier);
    return concatenated_string;
}



/**
 * char *get_identifier(char *fullPath, char *device);
 * 
 * Extracts the identifier from the full path by removing the device part
 *
 * This function removes the device string from the beginning of the full path,
 * leaving the identifier. It assumes that fullPath starts with device. Memory for the
 * identifier string is dynamically allocated; it's the caller’s responsibility to free
 * this memory when it is no longer needed.
 *
 * fullPath: A string representing the full path of the file
 * device: A string representing the device or base path
 * 
 * return: A pointer to a dynamically allocated string containing the identifier
 */
char *get_identifier(char *fullPath, char *device){

    // Allocate memory for the identifier string
    char *identifier = (char *)malloc((strlen(fullPath) - strlen(device) + 1) * sizeof(char));
    print_np_failure(identifier);

    // Copy the part of fullPath that comes after device into identifier
    strcpy(identifier, fullPath + strlen(device));
    return identifier;
}
