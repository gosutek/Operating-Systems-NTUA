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

void fork_procs(struct tree_node *node)
{
	int status;
	pid_t pid;
	pid_t* child_pids = malloc(node->nr_children * sizeof(pid_t));

	int numberOfChildren = node->nr_children;

	change_pname(node->name);
	printf("%s: init... , PID = %ld\n", node->name, (long) getpid());
	if (node->children != NULL) {		//If node != leaf
		while(numberOfChildren--) {		//for all children		
			pid = fork();		//Fork the process

			if (pid < 0) {		//Error handling
				perror("pronging error");
				exit(1);
			}
			else if(pid == 0) {	//children enter here
				fork_procs(node->children + numberOfChildren); //now im the root of my own tree!
			} else {
				//If this is the parent of the fork, save the child's pid and wait for it to stop
				child_pids[numberOfChildren-1] = pid;
				wait_for_ready_children(1);
			}
		}
	}

	//Raise the SIGSTOP signal to the process and stop it
	raise(SIGSTOP);

	//When the SIGCONT comes from the parent each node signals it's child nodes if it has
	numberOfChildren = node->nr_children;

	if (node->children != NULL) {
		while(numberOfChildren--) {
			kill(child_pids[numberOfChildren-1], SIGCONT);
           		//For each of the children processes that continue, the parent process waits for them to exit before it continues
			pid = wait(&status)> 0;
			//explain_wait_status(pid, status);
		}
	}

	printf("%s , PID = %ld, is finishing\n", node->name, (long)getpid());

	exit(0);
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	struct tree_node *root;

	if (argc < 2){
		fprintf(stderr, "Usage: %s <tree_file>\n", argv[0]);
		exit(1);
	}

	/* Read tree into memory */
	root = get_tree_from_file(argv[1]);

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* Child */
		fork_procs(root);
	}

	//Waiting the SIGSTOP from the child to continue
	wait_for_ready_children(1);

	show_pstree(pid);

	kill(pid, SIGCONT);

	/* Wait for the root of the process tree to terminate */
	wait(&status);
//	explain_wait_status(pid, status);

	printf("Main Process Finished\n");
	return 0;
}
