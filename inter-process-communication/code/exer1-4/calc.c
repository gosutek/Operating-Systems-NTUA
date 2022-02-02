#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "tree.h"
#include "proc-common.h"

void fork_procs(struct tree_node *node,int *pfd)
{
	ssize_t rd_error;
	if (node->children != NULL) {		//If node != leaf

		pid_t pid;
		int p0fd[2],p1fd[2];

		//Create pipes
		if (pipe(p0fd) < 0 || pipe(p1fd) < 0 ) {
			perror("Error creating pipe");
			exit(1);
		}

		//If the node has children it has two by the definition
		for (int i = 0; i < 2 ; i++) {
			pid = fork();		//Fork the process

			if (pid < 0) {		//Error handling
				perror("Error creating fork");
				exit(1);
			} else if(pid == 0) {	//children enter here
				//Each child closes the read endpoint of the pipe
				if (i == 0) {
					close(p0fd[0]);
					fork_procs(node->children + i,p0fd);
				} else if (i == 1) {
					close(p0fd[0]);
					fork_procs(node->children + i,p1fd);
				}
			}
		}

		//Parent code
		//Parent closes the write endpoint of the pipe
		close(p0fd[1]);
		close(p1fd[1]);

		int child0Value, child1Value;

		//We read the two values one for each pipe. These pipes block till bytes are written to the pipe
		rd_error = read(p0fd[0], &child0Value, sizeof(int));
		if (rd_error < 0) {
			perror("Reading error: ");
			exit(1);
		}
		close(p0fd[0]);

		rd_error = read(p1fd[0], &child1Value,sizeof(int));
		if (rd_error < 0) {
			perror("Reading error: ");
			exit(1);
		}
		close(p1fd[0]);

		//The result is calculated according to the operator on the node
		int result;

		if (strcmp("*",node->name) == 0) {
			result = child0Value * child1Value;
			printf("%d * %d = %d, PID = %ld\n", child0Value, child1Value, result, (long) getpid());
		} else if (strcmp("+",node->name) == 0) {
			result = child0Value + child1Value;
			printf("%d + %d = %d, PID = %ld\n", child0Value, child1Value, result, (long) getpid());
		} else {
			perror("Incorrect operator");
			exit(1);
		}

		//The current parent writes the result to its parent
		rd_error = write(pfd[1], &result , sizeof(int));
		if (rd_error < 0) {
			perror("Writing error: ");
			exit(1);
		}
	} else {
		//If we are at a leaf this should be a value so we pass this through the pipe
		int leafValue;
		sscanf(node->name, "%d", &leafValue);

		rd_error = write(pfd[1], &leafValue, sizeof(int));\
		if (rd_error < 0) {
			perror("Writing error");
			exit(1);
		}
		close(pfd[1]);
	}

	exit(0);
}

int main(int argc, char *argv[])
{
	ssize_t rd_error;
	pid_t pid;
	struct tree_node *root;

	if (argc < 2){
		fprintf(stderr, "Usage: %s <tree_file>\n", argv[0]);
		exit(1);
	}

	/* Read tree into memory */
	root = get_tree_from_file(argv[1]);

	int pfd[2];

	if (pipe(pfd) < 0) {
		perror("Error creating pipe");
		exit(1);
	}

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* Child */
		close(pfd[0]);
		fork_procs(root,pfd);
	}

	close(pfd[1]);

	int result;
	rd_error = read(pfd[0], &result, sizeof(int));
	if (rd_error < 0){
		perror("Reading error main: ");
		exit(1);
	}

	printf("Final Result = %d\n", result);

	return 0;
}
