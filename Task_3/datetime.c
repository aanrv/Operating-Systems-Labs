#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	// Create child
	pid_t ch = fork();
	if(ch == 0) {
		// In child
		printf("Child PID: %d\n", getpid());
		// Execute date command to print time and date
		execl("/bin/date", "date", (char*)NULL);
		// Parent process should terminate here
		perror("execl failure");
	}else {
		// Parent will wait for child to terminate
		if(wait(NULL) == -1) { perror(NULL); return EXIT_FAILURE; }
	}
	return EXIT_SUCCESS;
}
