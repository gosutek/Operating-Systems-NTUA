#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void iRead (const char *infile, char *buff[], int idx) {
        int fds;
        off_t fileSize = 0;
        ssize_t rcnt;
        fds = open(infile, O_RDONLY);
        if (fds == -1) {        //Opening error
                perror("Error while opening: ");
                close(fds);
                exit(-1);
        }
        //Get file size
        fileSize = lseek(fds, 0L, SEEK_END);            //Set the file potitional indicator at the end of file with offset 0
        lseek(fds, 0L, SEEK_SET);                       //Set the indicator at the start of the file with offset 0
        //Allocate memory for buffer
        buff[idx] = (char*) malloc(fileSize * sizeof(char));  //Allocate memory equal to the file size
        if (buff[idx] == NULL) {perror("Allocation error: "); close(fds); exit(-1);}
        // Read the file
        do {
                rcnt = read(fds, (void*) buff[idx], fileSize);
                if (fds == -1) {perror("Error while reading: "); close(fds); exit(-1);}
        } while(rcnt != 0);
        close(fds);
        return;
}

void writeFile (const char *outputFilename, char *inputFilename[], int numberOfFiles) {
        char **buffer;
        buffer = (char**) malloc((numberOfFiles - 1) * sizeof(char*)); //Array of strings for each input file
        ssize_t wcnt;
        for (int i = 1; i <= numberOfFiles - 1; i++) {
                iRead(inputFilename[i], buffer, i - 1);
        }
        int fdd = open(outputFilename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        for (int i = 0; i < numberOfFiles - 1; i++) {
                wcnt = write(fdd, buffer[i], strlen(buffer[i]));
                if (wcnt == -1) {perror("Error while writing: "); close(fdd); exit(-1);}
                free(buffer[i]);
        }
        close(fdd);
        free(buffer);
        return;
}
