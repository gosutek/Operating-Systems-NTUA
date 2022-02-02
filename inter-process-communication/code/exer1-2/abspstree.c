#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"

#define SLEEP_PROC_SEC 10
#define OBSV_SLEEP 1

void prong(struct tree_node *node) {
	int status;
	pid_t pid;
	int prongno = node->nr_children;
	if (node->children != NULL) {		//If node != leaf
		while(prongno--) {		//for all children		
			pid = fork();		//create them
			if (pid < 0) {		//Error handling
				printf("%s: i have a", node->name); //dont know if this works
				perror("pronging error");
				exit(1);
			}
			else if(pid == 0) {	//children enter here
				printf("%s: init...\n", (node->children + prongno)->name);
				prong(node->children + prongno); //now im the root of my own tree!
			}
		}
		sleep(OBSV_SLEEP); //give some time to reach end of tree
		printf("%s: waiting for all children to terminate...\n", node->name);
		while((pid = wait(&status)> 0));//wait for all children to finish. wait(&status) returns -1 for no children.
		printf("%s: all children terminated...exiting...\n", node->name);
		exit(1);
	}
	//leafs execute these
	printf("%s: im a leaf! and im sleeping!\n", node->name);
	sleep(SLEEP_PROC_SEC);
	printf("%s: im exiting...\n", node->name);
	exit(1);
}

int main(int argc, char *argv[]) {
	struct tree_node *root;
	pid_t pid;
	int status;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_tree_file>\n\n", argv[0]);
		exit(1);
	}

	root = get_tree_from_file(argv[1]);
	pid = fork(); 	//fork from main to create root process
	if (pid < 0) {
		perror("main fork error");
		exit(1);
	}
	else if (pid == 0){	//root process enters here
		prong(root);
	}
	pid = wait(&status);
	print_tree(root);

	return 0;
}