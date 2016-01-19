#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char** argv) {
	// At least one filename must be provided
	if(argc <= 1) {
		fprintf(stderr, "Usage: file1 [file2 file3 ...]\n");
		return EXIT_FAILURE;
	}
	
	// Buffer to hold file's data
	const size_t BUFFSIZE = 64;
	char buffer[BUFFSIZE];

	int i;
	for(i = 1; i < argc; ++i) {
		char* filename = argv[i];
		printf("\n===== %s =====\n", filename);
		int fd = open(filename, O_RDONLY);

		// Open file
		if(fd == -1) {
			fprintf(stderr, "Unable to open file: %s\n", filename);
			perror(NULL);
			continue;
		}

		// Read file and print buffer in blocks of `readSize` bytes
		// Starts at 0 because there will initially be no data in the buffer
		int readSize = 0;
		do {
			printf("%.*s", readSize, buffer);
			readSize = read(fd, buffer, BUFFSIZE);
			// After reading, only continue if data was read without errors
			// If read was successful, the next loop will print the data
			if(readSize == -1) {
				fprintf(stderr, "Unable to read file %s\n", filename);
				perror(NULL);
			}
		} while(readSize > 0);
		
		// Close file
		// If it fails, no point in exiting the whole program since
		// there are other files that might have been provided as arguments
		if(close(fd) == -1) {
			fprintf(stderr, "Unable to close file: %s\n", filename);
			perror(NULL);
		}
	}
	return EXIT_SUCCESS;
}

