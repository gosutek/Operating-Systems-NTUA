#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define INIT_PROC_SEC	1
#define SLEEP_TREE_SEC  2


void fork_procs(char *name, int exitno){
        printf("%s: Sleeping...\n", name);
        sleep(SLEEP_PROC_SEC);

        printf("%s: Exiting...\n", name);
        exit(exitno);
}

int main() {
        pid_t pid;
        int status;
	printf("A: Init...\n");
        pid = fork();		//A is created here.
        if (pid < 0) {
                perror("main: fork\n");	//Check errors forking father of A.
                exit(1);
        }
        else if (pid == 0) {		//pid_father == 0  means that this process is a child. In this case A.
		change_pname("A");
		printf("B: Init...\n");
		pid = fork();	//A forks to B.
		if (pid < 0) {
			perror("A: fork -> B\n");
			exit(1);
		}
		else if (pid == 0) { 	//B node will enter here.
			change_pname("B");
			printf("D: Init...\n");
			pid = fork();	//D is created here.
			if (pid < 0) {
				perror("B: fork\n");
				exit(1);
			}
			if (pid == 0) {	//D will enter here.
				change_pname("D");
				fork_procs("D", 13);
			}
			printf("B: Waiting for child to terminate...\n");
			change_pname("B");
			pid = wait(&status);
			explain_wait_status(pid, status);
			printf("B: Exiting...\n");
			exit(19);
		}	
		printf("C: Init...\n");
		pid = fork();	//C is created here.
		if (pid < 0) {
			perror("A: fork -> C\n");
			exit(1);
		}
		else if (pid == 0) {
			change_pname("C");
			fork_procs("C", 17);
		}
		printf("A: Waiting for child to terminate...\n");
		pid = wait(&status);
		explain_wait_status(pid, status);
		printf("A: Waiting for child to terminate...\n");
		pid = wait(&status);
		explain_wait_status(pid, status);
		printf("A: Exiting...\n");
		exit(16);
        }
	sleep(SLEEP_TREE_SEC);
	show_pstree(getpid());
	pid = wait(&status);
        explain_wait_status(pid, status);

        return 0;
}
