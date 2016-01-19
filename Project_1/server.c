#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

int main(int argc, char** argv) {
	if(argc != 4) {
		fprintf(stderr, "Not enough arguments.\n");
		exit(EXIT_FAILURE);
	}

	// convert strings to longs
	long opOne = strtol(argv[2], NULL, 10);
	long opTwo = strtol(argv[3], NULL, 10);
	
	const char plus[2] = "+";
	const char minus[2] = "-";
	// If user wants subtraction, negate second operand
	if(strcmp(argv[1], minus) == 0) {
		opTwo *= -1;
	}else if(strcmp(argv[1], plus) != 0) {
		fprintf(stderr, "Invalid operation.\n");
		exit(EXIT_FAILURE);
	}
	
	long answer = opOne + opTwo;
	// buffer to store string of answer
	const size_t strsize = 64;
	char resultstr[strsize];
	// convert long to string
	size_t sz = snprintf(resultstr, strsize-1, "%ld", answer);
	// write string to file descriptor of 1
	// if this is cloned in server, this will not write to stdout, but to fd[1] because a pipe is created
	// the answer will then be retrieved from fd[0] by the parent and printed to stdout
	// this way, numbers larger than 8 bits can be transmitted
	write(1, resultstr, sz+1);
	exit(EXIT_SUCCESS);
}

