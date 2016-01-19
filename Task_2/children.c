#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
	pid_t child_one;
	pid_t child_two;

	// Create first child
	child_one = fork();

	// If 0 then child process, otherwise parent
	if(child_one == 0) {
		// In child process
		printf("I am child one, my pid is: %d\n", getpid());
	}else {
		// Create a second child from within parent process
		child_two = fork();
		if(child_two == 0) {
			// In second child process
			printf("I am child two, my pid is: %d\n", getpid());
		}else {
			// In parent process
			// Wait for each child to exit
			pid_t wpid = 0;
			while(wait(&wpid) != -1)
				;
			// Make sure the last error says
			// there were no children to wait for
			// in parent process
			if(errno != ECHILD) {
				perror(NULL);
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

