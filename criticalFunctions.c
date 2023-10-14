#define _POSIX_C_SOURCE 200809L   
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
 * read_directory(char *device, char *identifier);
 * 
 * Main function which reads files from a specified directory into allFileSet
 *
 * Its behavior of selecting which files will read depends on the global flags that are set
 *
 * device: A string representing the device or base path of the directory to read
 * identifier: A string representing the name (path relative to device) of the directory to read
 */
void read_directory(char *device, char *name){
    struct dirent *entry;
    struct stat entryStat; // To store status info of file or directory.
    

    // Get the full directory path and open directory
    char* fullDirPath = get_full_path(device, name);
    DIR *dir = opendir(fullDirPath);
    print_np_failure(dir);
    
    // Iterate over entries in directory.
    while ((entry = readdir(dir)) != NULL){
        // Skip "." and ".." directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        
        // Skip hidden files if flag_all_files is false
        if (!flag_all_files && entry->d_name[0] == '.'){
            continue;
        }

        

        // -o regex filter
        if (flag_find_pattern){
            bool oFound = false;
            for (int i=0; i<numOpatterns; i++){
                if (regexec(oRegexList[i], entry->d_name, 0, NULL, 0) != REG_NOMATCH){
                    oFound = true;
                    break;
                }
            }
            if (!oFound){
                continue;
            }
        }


        // -i regex filter
        if (flag_ignore_pattern){
            bool iFound = false;
            for (int i=0; i<numIpatterns; i++){
                if (regexec(iRegexList[i], entry->d_name, 0, NULL, 0) != REG_NOMATCH){
                    iFound = true;
                    break;
                }
            }
            if (iFound){
                continue;
            }
        }



        // Get the full path of entry, its identifier, and information about it
        char* fullEntryPath = get_full_path(fullDirPath, entry->d_name);
        stat(fullEntryPath, &entryStat);
        char* entryIdentifier = get_identifier(fullEntryPath, device);


        // If recursive flag is on and entry is a directory
        if (S_ISDIR(entryStat.st_mode) && flag_recursive){

            // construct new path and recursively call read_directory on the newly found directory entry
            read_directory(device, entryIdentifier);
            continue;

        } else if (S_ISREG(entryStat.st_mode)){ // else if it is a file

            // if allFileSet is full, double the container size
            if (numFilesDiscovered == containerSize){
                containerSize *= 2;
                allFileSet = realloc(allFileSet, containerSize * sizeof(struct File *));
                print_np_failure(allFileSet);
            }

            // Initialize File object with entry info and update allFileSet
            struct File *fp = (struct File *)malloc(sizeof(struct File));
            print_np_failure(fp);
            fp->identifier = strdup(entryIdentifier);
            fp->modtime = entryStat.st_mtime;
            fp->device = strdup(device);
            update_allFileSet(fp);
        }

        free(fullEntryPath);
        free(entryIdentifier);
    }
    free(fullDirPath);
    closedir(dir);
}


/**
 * void synchronise(char *device);
 * 
 * Synchronises files in the specified device with the latest versions in allFileSet
 *
 * device: A string representing the device or base path to be synced with the latest versions
 */
void synchronise(char *device){
    // initialisation of stat buffer
    struct stat fileStat;
    
    // Iterate over files in allFileSet
    for (int i=0; i < numFilesDiscovered; i++){
        struct File *latestFile = allFileSet[i]; // current File in iteration.
        print_np_failure(latestFile);
        
        // Construct the full file path for the target file and the latest version
        char* fullFilePath_target = get_full_path(device, latestFile->identifier);
        char* fullFilePath_latest = get_full_path(latestFile->device, latestFile->identifier);

        // Check if the file exists on the local device.
        if (does_file_exist(device, latestFile)){
            // get mod times
            stat(fullFilePath_target, &fileStat);
            // If mod times are different, copy latest version to the local file
            if (fileStat.st_mtime != latestFile->modtime){
                copy_file_contents(fullFilePath_latest, fullFilePath_target);
                if (flag_permissions){
                    copy_attr(fullFilePath_latest, fullFilePath_target);
                }
            } 
        } else { // else if the file doesn't exist on the local device

            char* parentPath = get_parent_directory(fullFilePath_target);
            if (does_directory_exist(parentPath)){ // if parent pathway exists
                // create latest version in local device
                copy_file_contents(fullFilePath_latest, fullFilePath_target);
                if (flag_permissions){
                    copy_attr(fullFilePath_latest, fullFilePath_target);
                }
            } else {
                // make parent pathway and create latest version in local device
                construct_path(parentPath);
                copy_file_contents(fullFilePath_latest, fullFilePath_target);
                if (flag_permissions){
                    copy_attr(fullFilePath_latest, fullFilePath_target);
                }
            }
            free(parentPath);
        }


        // Free the dynamically allocated memory for the file paths.
        free(fullFilePath_latest);
        free(fullFilePath_target);
    }
}



/**
 * void copy_attr(char *source_path, char *dest_path);
 * 
 * Copies the modtime and file permissions from one file to another
 *
 * source_path: string representing full path of source file
 * dest_path: string representing full path of destination file
 */
void copy_attr(char *source_path, char *dest_path){
    struct stat source_stat;
    struct utimbuf new_times;

    // Get the source file's attributes
    stat(source_path, &source_stat);

    // Copy the source file's permissions to the destination file
    chmod(dest_path, source_stat.st_mode);

    new_times.actime = source_stat.st_atime;  // set access time
    new_times.modtime = source_stat.st_mtime;  // set modification time

    // Copy the source file's modtime to the destination file
    utime(dest_path, &new_times);
}



/**
 * void update_allFileSet(struct File *fp);
 * 
 * Appends the allFileSet with a new File object
 * If allFileSet already contains File with the same identifier,
 * the File object inside the set is updated with the latest modtime and device
 *
 * fp: A pointer to a File structure to be added to allFileSet
 */
void update_allFileSet(struct File *fp){

    // Iterate over files in allFileSet
    for (int i=0; i<numFilesDiscovered; i++){
        // Check if match is found
        if (strcmp(fp->identifier, allFileSet[i]->identifier) == 0){

            // update the allFileSet object with the latest mod time and device
            if (allFileSet[i]->modtime < fp->modtime){
                allFileSet[i]->modtime = fp->modtime;
                allFileSet[i]->device = fp->device;
                free(fp);
                return;
            } else if (allFileSet[i]->modtime >= fp->modtime){
                free(fp);
                return;
            }
        }
    }
    // If no match is found, append the File object to the allFileSet array
    allFileSet[numFilesDiscovered] = fp;
    numFilesDiscovered++;
}


/**
 * bool does_file_exist(char *device, struct File *fp);
 *
 * Checks if a specified File object from allFileSet exists in the specified device
 * 
 * This function takes a device string and a pointer to a File object
 * It returns true if the file exists, and false otherwise
 *
 * device: A string representing the device or base path
 * fp: A pointer to a File object
 *
 * return: Returns true if the file exists in the specified device; otherwise, returns false.
 */
bool does_file_exist(char *device, struct File *fp){

    // If the File's recorded device matches the provided device, file can be assumed to exist
    if (strcmp(fp->device, device) == 0){
        return true;
    }

    // initialisation of stat buffer and full path string
    struct stat buffer;
    char *fullPath = get_full_path(device, fp->identifier);


    // Check if the file at the full path exists
    int exist = stat(fullPath, &buffer);
    free(fullPath);
    if (exist == 0)
        return true;
    else
        return false;
}

/**
 * void copy_file_contents(char *sourcePath, char *destPath);
 *
 * This function copies contents of source file into the destination file.
 * Works for all types of regular files such as txt files and binaries
 *
 * sourcePath: A path string representing the source file
 *
 * destPath A path string representing the source file
 *             If the destination does not exist, this function will create it
 *             If the destination exists, this function will overwrite it
 */
void copy_file_contents(char *sourcePath, char *destPath){
    FILE *src_file, *dest_file;
    size_t buffer_size = 1024;  // buffer size.
    char buffer[buffer_size];   // Buffer to hold chunks of data
    size_t bytes_read;          // Number of bytes read in a single fread() call
    
    // Open source and destination files
    src_file = fopen(sourcePath, "rb");
    print_np_failure(src_file);
    dest_file = fopen(destPath, "wb");
    print_np_failure(dest_file);

    // Read chunks of data from source file and write to destination file
    while ((bytes_read = fread(buffer, 1, buffer_size, src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dest_file);
    }

    fclose(src_file);
    fclose(dest_file);
}


/**
 * bool does_directory_exist(char *dirPath);
 *
 * This function checks if given pathway to a directory exists on the system
 *
 * dirPath: A path string representing the pathway to the directory
 *
 * return: a boolean that indicates whether directory exists
 */
bool does_directory_exist(char *dirPath){
    // initialisation of stat object
    struct stat statbuf;
    stat(dirPath, &statbuf);
    
    return S_ISDIR(statbuf.st_mode);
}

/**
 * void construct_path(char *dirPath);
 *
 * This function constructs a pathway to a directory on the system recursively
 *
 * dirPath: A path string representing the pathway to the directory
 */
void construct_path(char *dirPath){

    if (!does_directory_exist(dirPath)){
        // If directory does not exist, get the path of its parent directory
        char* parentPath = get_parent_directory(dirPath);
        
        // Recursively call construct_path() to attempt creation of the parent directory
        // This will continue recursively until a directory that exists is found
        construct_path(parentPath);
        
        // Attempt to create the parent directory with the specified mode
        mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
        mkdir(dirPath, mode);

        // Free the memory allocated for the parentPath string to avoid memory leaks
        free(parentPath);
        
        // End the function call and return to the previous level of recursion
        return;
    } else {
        // If the directory already exists, simply return without doing anything
        return;
    }
}

