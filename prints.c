#include <stdio.h>
#include <stdlib.h>
#include "mysync.h"

/**
 * void print_devices();
 * 
 * Prints the names of all recorded devices to the console
 */
void print_devices(){
    // Introduction message
    printf("\nThe devices recorded are:\n");
    
    // Print each device in the deviceList array
    for (int i=0; i<deviceCount; i++){
        printf("%s\n", deviceList[i]);
    }
}

/**
 * void print_file(struct File *fp);
 * 
 * Prints information (identifier and modtime) about a provided File object to the console
 *
 * fp: A pointer to a File structure containing the file information to be printed
 */
void print_file(struct File *fp){
    printf("%-50s modtime: %20li\n", fp->identifier, fp->modtime);
}

/**
 * void print_all_files();
 * 
 * Prints information about all files inside allFileSet
 * each line output is identifier and modtime of a file
 */
void print_all_files(){
    printf("\nThe following are all entries recorded within allFileSet...\n");
    for (int i=0; i<numFilesDiscovered; i++){
        print_file(allFileSet[i]);
    }
}

/**
 * void print_mem_allocation_failure(void *pointer);
 * 
 * Checks if the pointer is NULL. If so, prints our memory allocation failure message
 */
void print_np_failure(void *pointer){
    if (!pointer){
        perror("Null pointer encountered. Could be due to memory allocation failure\n");
        exit(EXIT_FAILURE);
    }    
}

