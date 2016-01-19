#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	pid_t ch = fork();
	if(ch == 0) {
		// In child
		printf("Child PID: %d\n", getpid());
		// execvp takes an array as its second argument
		char* args[] = {"ls", "-la", (char*)NULL};
		// execvp searches all directories in PATH variable
		// to execute command so path needn't be provided
		execvp("ls", args);
		// Child should terminate
		perror("execvp failure");
	}else {
		// Parent will wait for child to terminate before exiting
		if(wait(NULL) == -1) { perror(NULL); return EXIT_FAILURE; }
	}
	return EXIT_SUCCESS;
}

