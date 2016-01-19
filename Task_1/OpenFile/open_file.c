#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char** argv) {
	// Must provide at least 1 argument
	if(argc <= 1) {
		fprintf(stderr, "Usage: file 1 [file2 file3 ...]\n");
		return EXIT_FAILURE;
	}

	// Create and open file(s)
	int i;
	for(i = 1; i < argc; ++i) {
		char* filename = argv[i];
		// Open file with read write permissions
		// Create it with -rw-r--r-- permissions if it doesn't already exist
		int fd;
		if(fd = open(filename, O_RDWR | O_CREAT, 00644) == -1) {
			fprintf(stderr, "Unable to open file: %s\n", filename);
			perror(NULL);
			continue;
		}
		printf("File: %s was successfully opened.\n", filename);
		// Close file
		if(close(fd) == -1) {
			fprintf(stderr, "Unable to close file: %s.\n", filename);
			perror(NULL);
		}
	}
	return EXIT_SUCCESS;
}

