#include "fconc.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
        if (argc < 3) {
                printf("Usage: %s inputfile1 inputfile2 [outputfile](optional)\n", argv[0]);
                exit(-1);
        }
        if (argv[argc - 1] == NULL) {argv[argc] = "fconc.out";}
        int fdd = open(argv[argc - 1], O_APPEND | O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        static const int BUFFER_N = 1024;
        char buff [BUFFER_N];
        for (int i = 1; i < argc - 1; i++){
        iRead(argv[i], buff, fdd);
}
        close(fdd);
        return 0;
}
