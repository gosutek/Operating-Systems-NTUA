#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void iRead (const char *infile, const char *buff, int fdd) {
        int fds;
        ssize_t wcnt, rcnt;
        fds = open(infile, O_RDONLY);
        if (fds == -1) {printf("No such file or directory %s\n", infile); exit(-1);}
        rcnt = read(fds, (void*) buff, 1);
        do {
                if (fds == -1) {printf("Error while reading\n"); exit(-1);}
                wcnt = write(fdd, buff, strlen(buff));
                if (wcnt == -1) {printf("Error while writing\n"); exit(-1);}
                rcnt = read(fds, (void*) buff, strlen(buff));
        } while(rcnt != 0);
        close(fds);
        return;
}
