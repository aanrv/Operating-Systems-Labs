// Note:
// All files should be in same folder as executable

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
	const char firstCommand[] = "Prcs_P1";
	const char secondCommand[] = "Prcs_P2";
	
	pid_t ch1 = fork();
	if(ch1 == 0) {
		// In first child
		pid_t ch2 = fork();
		if(ch2 == 0) {
			// In first child's first child
			execl("./Prcs_P1", "Prcs_P1", (char*)NULL);
			perror("Make sure Prcs_P1 is in current working directory");
			exit(EXIT_FAILURE);
		}else {
			// Wait for child to create files
			// Check to make sure it was successful
			int status = 0;
			waitpid(-1, &status, 0);
			// If program exited with failure, don't perform Prcs_P2
			if(WEXITSTATUS(status) == EXIT_FAILURE) exit(EXIT_FAILURE);
			
			// After first files have been created
			// write to them accordingly
			execl("./Prcs_P2", "Prcs_P2", (char*)NULL);
			perror("Make sure Prcs_P2 is in current working directory");
			exit(EXIT_FAILURE);
		}
	}else {
		// Wait for child to terminate
		// before exiting
		wait(NULL);
	}
	return EXIT_SUCCESS;
}

