#include "fconc.h"
#include <stdio.h>
#include <stdlib.h>

//This is a driver program for the functions writeFile and iRead
//The goal is to concatenate 2 text files into an output file
//The idea is read all the files one by one into separate, dynamically allocated buffers in a buffer array
//then write those buffers to the output file.
//Works if given a file, both as input and output!

int main(int argc, char *argv[]) {
        if (argc != 3 && argc != 4) {   //Usage error
                printf("Usage: %s inputfile1 inputfile2 [outputfile](optional)\n", argv[0]);
                exit(-1);
        }
        char *outputFilename;
        if (argv[3] == NULL) {
                outputFilename = "fconc.out";   //Default outputFilename
                writeFile(outputFilename, argv, argc);
        }
        else {
                outputFilename = argv[3];
                writeFile(outputFilename, argv, argc - 1);
        }
        return 0;
}
