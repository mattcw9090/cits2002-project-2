//  CITS2002 Project 2 2023
//  Student1:   22974799   Samuel Chew
//  Student2:   22974046   Matthew Chew


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "mysync.h"


int main(int argc, char *argv[]){
    int opt;

    // parsing of options into flags
    while ((opt = getopt(argc, argv, "ari:o:npv")) != -1) {
        switch (opt) {
            case 'a':
                flag_all_files = true;
                break;
            case 'r':
                flag_recursive = true;
                break;
            case 'i':
                flag_ignore_pattern = true;
                // resize iGlobs array
                iGlobs = (char**) realloc(iGlobs, (numIpatterns+1) * sizeof(char *));
                print_np_failure(iGlobs);

                // store string argumnent for -i in iGlobs array
                char* iglob = (char*) malloc((strlen(optarg) + 1) * sizeof(char));
                print_np_failure(iglob);
                strcpy(iglob, optarg);
                iGlobs[numIpatterns] = iglob;
                numIpatterns += 1;
                break;
            case 'o':
                flag_find_pattern = true;
                // resize oGlobs array
                oGlobs = (char**) realloc(oGlobs, (numOpatterns+1) * sizeof(char *));
                print_np_failure(oGlobs);

                // store string argumnent for -o in oGlobs array
                char* oglob = (char*) malloc((strlen(optarg) + 1) * sizeof(char));
                print_np_failure(oglob);
                strcpy(oglob, optarg);
                oGlobs[numOpatterns] = oglob;
                numOpatterns += 1;
                break;
            case 'p':
                flag_permissions = true;
                break;
            case 'n':
                flag_identify = true;
                flag_verbose = true;
                break;
            case 'v':
                flag_verbose = true;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }
    

    // initialisation of regex structures
    iRegexList = (regex_t **)malloc(numIpatterns* sizeof(regex_t *));
    print_np_failure(iRegexList);
    oRegexList = (regex_t **)malloc(numOpatterns* sizeof(regex_t *));
    print_np_failure(iRegexList);
    compile_globs(iGlobs, iRegexList, numIpatterns);
    compile_globs(oGlobs, oRegexList, numOpatterns);



    // initilisation of devices
    deviceCount = argc - optind;
    deviceList = (char**)malloc(deviceCount * sizeof(char *));
    for (int i = 0; i < deviceCount; i++) {
        deviceList[i] = argv[optind];
        optind++;    
    }


    // verbose: print device paths
    if (flag_verbose){
        print_devices();
    }


    // initialisation and filling of allFileSet
    allFileSet = (struct File **)malloc(10 * sizeof(struct File *));
    for (int i=0; i < deviceCount; i++){  
        read_directory(deviceList[i], "/");
    }


    // verbose: print allFileSet
    if (flag_verbose){
        print_all_files();
    }  


    // if flag_identify is false, synchronise
    if (!flag_identify){
        // synchronisation of each device with allFileSet
        for (int i=0; i < deviceCount; i++){
            synchronise(deviceList[i]);
        }
    }


    // deallocation of heap memory for globals
    for(int i=0; i<numFilesDiscovered; i++){
        free(allFileSet[i]->identifier);
        free(allFileSet[i]->device);
        free(allFileSet[i]);
    }
    free(allFileSet);
    free(deviceList);
    for (int i=0; i<numIpatterns; i++){
        regfree(iRegexList[i]);
        free(iRegexList[i]);
    }
    free(iRegexList);
    for (int i=0; i<numOpatterns; i++){
        regfree(oRegexList[i]);
        free(oRegexList[i]);
    }
    free(oRegexList);
    printf("\nProgram ran sucessfully\n\n");
    exit(EXIT_SUCCESS);
}