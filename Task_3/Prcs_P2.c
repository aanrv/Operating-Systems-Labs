#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFSIZE 100

// Replaces all occurences of `original` with `replacement`
// in `buffer` of size `buffersize`
void replaceChar(char original, char replacement, char* buffer, int buffersize);

int main(void) {
	int exitVal = EXIT_SUCCESS;

	// Filenames
	const char readfile[] = "source.txt";
	const char write1[] = "destination1.txt";
	const char write2[] = "destination2.txt";
	
	const size_t firstReadSize = 100;
	const size_t secondReadSize = 50;
	// Will hold file data
	char buffer[BUFFSIZE];
	// 0 init array
	int i;
	for(i = 0; i < BUFFSIZE; ++i) buffer[i] = 0;
	
	// Open files while checking for errors
	int src = open(readfile, O_RDONLY);
	if(src == -1) {
		perror("Error opening source file");
		return EXIT_FAILURE;
	}
	int d1 = open(write1, O_WRONLY);
	if(d1 == -1) {
		perror("Error opening destionation1 file");
		// Manually close previously opened files
		if(close(src) == -1) perror(NULL);
		return EXIT_FAILURE;
	}
	int d2 = open(write2, O_WRONLY);
	if(d2 == -1) {
		perror("Error opening destination2 file");
		// Manually close previously opened files
		if(close(src) == -1) perror(NULL);
		if(close(d1) == -1) perror(NULL);
		return EXIT_FAILURE;
	}
	
	// Read and write files accordingly
	ssize_t readSize;
	do {
		// Read next <=100 bytes from source
		readSize = read(src, buffer, firstReadSize);
		if(readSize == -1) goto exit_error;
		// Replace 1's with A's in buffer
		replaceChar('1', 'A', buffer, readSize);
		// Terminate program if issue writing
		if(write(d1, buffer, readSize) == -1) goto exit_error;

		// Read next <=50 bytes from source
		readSize = read(src, buffer, secondReadSize);
		if(readSize == -1) goto exit_error;
		// Replace 2's with B's in buffer
		replaceChar('2', 'B', buffer, readSize);
		if(write(d2, buffer, readSize) == -1) goto exit_error;
	} while(readSize > 0);
	
	// Program was successful
	goto exit_close;
exit_error:
	// print last error and modify return value
	perror(NULL);
	exitVal = EXIT_FAILURE;
exit_close:
	// attempt to close all files
	if(close(src) == -1) perror("Unable to close source file");
	if(close(d1) == -1) perror("Unable to close destination1 file");
	if(close(d2) == -1) perror("Unable to close destination2 file");

	return exitVal;
}

void replaceChar(char original, char replacement, char* buffer, int buffersize) {
	while(buffersize-- > 0) {
		if(*buffer == original) *buffer = replacement;
		++buffer;
	}
}

