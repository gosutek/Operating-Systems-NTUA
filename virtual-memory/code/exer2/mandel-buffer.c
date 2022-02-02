/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;
/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;


void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s process_count \n\n"
		"Exactly one argument required:\n"
		"    process_count: The number of processes to create.\n",
		argv0);
	exit(1);
}

int safe_atoi(char *s, int *val)
{
	long l;
	char *endp;

	l = strtol(s, &endp, 10);
	if (s != endp && *endp == '\0') {
		*val = l;
		return 0;
	} else
		return -1;
}

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
	/*
	 * x and y traverse the complex plane.
	 */
	double x, y;

	int n;
	int val;

	/* Find out the y value corresponding to this line */
	y = ymax - ystep * line;

	/* and iterate for all points on this line */
	for (x = xmin, n = 0; n < x_chars; x+= xstep, n++) {

		/* Compute the point's color value */
		val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
		if (val > 255)
			val = 255;

		/* And store it in the color_val[] array */
		val = xterm_color(val);
		color_val[n] = val;
	}
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
	int i;

	char point ='@';
	char newline='\n';

	for (i = 0; i < x_chars; i++) {
		/* Set the current color, then output the point */
		set_xterm_color(fd, color_val[i]);
		if (write(fd, &point, 1) != 1) {
			perror("compute_and_output_mandel_line: write point");
			exit(1);
		}
	}

	/* Now that the line is done, output a newline character */
	if (write(fd, &newline, 1) != 1) {
		perror("compute_and_output_mandel_line: write newline");
		exit(1);
	}
}

void compute_and_store_mandel_line(int fd, int line, int processNumber, int processCount, int** buffer)
{
	/*
	 * A temporary array, used to hold color values for the line being drawn
	 */
	int color_val[x_chars];


	compute_mandel_line(line, color_val);

	int i;

	for (i=0;i<x_chars;i++) {
		buffer[line][i] = color_val[i];
	}
}


void process_fn(int processNumber, int processCount, int** buffer) {
	int line;
	for (line = processNumber; line < y_chars; line += processCount) {
		compute_and_store_mandel_line(1, line, processNumber, processCount, buffer);
	}

	return;
}

/*
 * Create a shared memory area, usable by all descendants of the calling
 * process.
 */
void *create_shared_memory_area(unsigned int numbytes)
{
        int pages, totalPageBytes;
        void *addr;

        if (numbytes == 0) {
                fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
                exit(1);
        }

        /*
         * Determine the number of pages needed, round up the requested number of
         * pages
         */
        pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;
        totalPageBytes = pages * sysconf(_SC_PAGE_SIZE);

        /* Create a shared, anonymous mapping for this number of pages */
        addr = mmap(NULL, totalPageBytes, PROT_READ | PROT_WRITE , MAP_SHARED | MAP_ANONYMOUS, -1, 0 );

        return addr;
}

void destroy_shared_memory_area(void *addr, unsigned int numbytes) {
        int pages;

        if (numbytes == 0) {
                fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
                exit(1);
        }

        /*
         * Determine the number of pages needed, round up the requested number of
         * pages
         */
        pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

        if (munmap(addr, pages * sysconf(_SC_PAGE_SIZE)) == -1) {
                perror("destroy_shared_memory_area: munmap failed");
                exit(1);
        }
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		usage(argv[0]);


    int processCount;

	if (safe_atoi(argv[1], &processCount) < 0 || processCount <= 0) {
		fprintf(stderr, "`%s' is not valid for `process_count'\n", argv[1]);
		exit(1);
	}

	xstep = (xmax - xmin) / x_chars;
	ystep = (ymax - ymin) / y_chars;


	//Initialize buffer in shared memory
	int** buffer = (int**) create_shared_memory_area(x_chars*sizeof(int*));

	int i;
	for (i=0;i<x_chars;i++) {
		buffer[i] = (int*) create_shared_memory_area(y_chars*sizeof(int));
	}

	pid_t pid;
	int status;

	for (i = 0; i < processCount; i++) {
		pid = fork(); 	//fork from main to create child process
		if (pid < 0) {
			perror("Fork error");
			exit(1);
		} else if (pid == 0) {
			process_fn(i, processCount, buffer);
			exit(0);
		}
	}

	while ((pid = wait(&status) > 0));

	//Outputs mandel line
	//We loop through all the lines of the buffer and print them one by one
	int line;
	for (line=0;line<y_chars;line++) {
		int color_val[x_chars];
		for (i=0;i<x_chars;i++) {
			color_val[i] = buffer[line][i];
		}

		output_mandel_line(1, color_val);
	}

	//Clear shared memory
	for (i=0;i<x_chars;i++) {
		destroy_shared_memory_area(buffer[i], y_chars*sizeof(int));
	}
	destroy_shared_memory_area(buffer, x_chars*sizeof(int));

	reset_xterm_color(1);

	return 0;
}
