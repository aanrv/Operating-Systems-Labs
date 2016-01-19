#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(void) {
	// Names of files to be created
	const char file1[] = "destination1.txt";
	const char file2[] = "destination2.txt";
	
	pid_t ch1 = fork();
	if(ch1 == 0) {
		// In first child
		pid_t ch2 = fork();
		if(ch2 == 0) {
			// In second child
			// Create files
			execlp("touch", "touch", file1, file2, (char*)NULL);
			// Child should terminate here
			perror("execlp failure in ch2");
		}else {
			// Wait for ch2 to create files before
			// attempting to modify permission
			if(wait(NULL) == -1) perror(NULL);
			// Modify permissions for user
			execlp("chmod", "chmod", "u+rwx", file1, file2, (char*)NULL);
			// Process should terminate here
			perror("execlp failure in ch1");
		}
	}else {
		// Parent will wait for child to terminate
		if(wait(NULL) == -1) { perror(NULL); return EXIT_FAILURE; }
	}

	return EXIT_SUCCESS;
}

