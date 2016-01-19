#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 	// open
#include <unistd.h>	// read, write
#include <errno.h>
#include <fcntl.h>	// O_RDWR

int main(int argc, char** argv) {
	if(argc != 3) {
		fprintf(stderr, "Usage: sourcefile destinationfile\n");
		return EXIT_FAILURE;
	}
	
	char* source = argv[1];
	char* dest = argv[2];
	
	// Open files
	int sd = open(source, O_RDONLY);
	if(sd == -1) {
		perror("Unable to open source file\n");
		return EXIT_FAILURE;
	}
	
	// Open destination file
	// Create with rw-r--r-- permissions if necessary
	int dd = open(dest, O_CREAT | O_WRONLY, 00644);
	if(dd == -1) {
		perror("Unable to open destination file\n");
		if(close(sd) == -1) perror("Unable to close source file\n");
		return EXIT_FAILURE;
	}
	
	// Buffer will be used to read and write data
	const size_t BUFFER_SIZE = 100;
	char buffer[BUFFER_SIZE];
	
	const size_t STR_SIZE = 4;
	char str[] = "XYZ";
	
	ssize_t readSize;
	do {
		readSize = read(sd, buffer, BUFFER_SIZE);
		// If empty file, finish program
		// If error when reading, exit with failure
		if(readSize == 0) break;
		else if(readSize == -1) goto exit_error;
		
		// Replace '1's with 'A's
		int i;
		for(i = 0; i < BUFFER_SIZE; ++i) {
			if(buffer[i] == '1') buffer[i] = 'A';
		}
		// Write readSize bytes to the destination file
		// Note: `readSize` will not always be 100
		if(write(dd, buffer, readSize) == -1) goto exit_error;
		// Write XYZ string to file
		if(write(dd, str, STR_SIZE-1) == -1) goto exit_error;
	}while(readSize > 0);
	
	if(close(sd) == -1) perror("Error closing source file\n");
	if(close(dd) == -1) perror("Error closing destination file\n");
	return EXIT_SUCCESS;

exit_error:
	perror(NULL);
	if(close(sd) == -1) perror("Error closing source file\n");
	if(close(dd) == -1) perror("Error closing destination file\n");
	return EXIT_FAILURE;
}

