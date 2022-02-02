#include "Nfconc.h"
#include <stdio.h>
#include <stdlib.h>

//This is a driver program for the functions writeFile and iRead.
//The goal is to concatenate N text files into an output file.
//The idea is read all the files one by one into separate, dynamically allocated buffers in a buffer array
//then write those buffers to the output file.
//Works if given a file, both as input and output!
//Will always write to the last file given as argument.

int main(int argc, char *argv[]) {
        if (argc < 3) {   //Usage error
                printf("Usage: %s inputfile1 inputfile2 ... inputfileN [outputfile](required)\n", argv[0]);
                exit(-1);
        }
        char *outputFilename;
        outputFilename = argv[argc - 1];
        printf("Created/overwritten file: %s\n", outputFilename);
        writeFile(outputFilename, argv, argc - 1);
        return 0;
}
