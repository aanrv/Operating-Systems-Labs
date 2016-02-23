#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char** argv) {
	// Must be provided exactly two arguments
	if(argc != 3) {
		fprintf(stderr, "Usage: file destination\n");
		return EXIT_FAILURE;
	}
	
	// Retreive arguments
	char* source = argv[1];
	char* destination = argv[2];
	
	// Open source file for reading
	int sourceFD = open(source, O_RDONLY);
	if(sourceFD == -1) {
		fprintf(stderr, "Unable to open file: %s\n", source);
		perror(NULL);
		return EXIT_FAILURE;
	}
	// Open a file for writing, and create it with rw-r--r-- permissions if necessary
	int destinationFD = open(destination, O_WRONLY | O_CREAT, 00644);
	if(destinationFD == -1) {
		fprintf(stderr, "Unable to open file: %s\n", destination);
		perror(NULL);
		close(sourceFD);
		return EXIT_FAILURE;
	}
	
	// Create a buffer to hold temporarily read data from source file
	const size_t BUFFSIZE = 128;
	char* buffer[BUFFSIZE];
	// Initialized to 0 because write should write 0 bytes the first loop
	int readSize = 0;
	int writeSize;
	
	// Read from source and write to destination
	// until EOF or read/write error
	do {
		// Write `readSize` bytes to destination from `buffer`
		writeSize = write(destinationFD, buffer, readSize);
		if(writeSize == -1) perror("Unable to write file.\n");

		// Read next `BUFFSIZE` bytes and store in `buffer`
		readSize = read(sourceFD, buffer, BUFFSIZE);
		if(readSize == -1) perror("Unable to read file.\n");
	}while(readSize > 0 && writeSize != -1);
	
	// Close files
	if(close(sourceFD) == -1) perror("Unable to close source file.\n");
	if(close(destinationFD) == -1) perror("Unable to close destination file.\n");
	
	// If read or write failed, program exits with error
	if(readSize == -1 || writeSize == -1) return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

