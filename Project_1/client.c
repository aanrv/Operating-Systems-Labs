// Note: If operand arguments are too large, result may overflow.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	// fork()
#include <string.h>	// strcmp()
#include <limits.h>	// MAX_LONG
#include <errno.h>	// perror()

// Copies src to dest until terminating character or null terminater or destMax is reached
// Returns number of characters copied exluding null terminator
int strcpyterm(char* dest, const char* src, char term, int destMax);

// Returns 1 if str represent a base 10 integer
int isValidNum(char* str);

int main(void) {
	int ret = EXIT_SUCCESS;

	size_t buffersize = 64;
	// Buffers to store arguments
	char* operatorInput = malloc(sizeof(char) * (2));
	char* operandOne = malloc(sizeof(char) * (buffersize));
	char* operandTwo = malloc(sizeof(char) * (buffersize));

	// Hold user input
	char* inputBuffer = malloc(sizeof(char) * buffersize);
	// Hold result
	size_t resultbuffsize = 64;
	char* resultbuffer = malloc(sizeof(char) * resultbuffsize);

	while (1) {
		printf("\nPlease enter a statement in the following format:\n"
				"operator(+|-) operand1 operand2\n"
				"(Enter an empty line to exit)\n");

		// Read line
		size_t previousBuffersize = buffersize;
		ssize_t readSize = getline(&inputBuffer, &buffersize, stdin);

		// Make sure no errors and input is not larger than the intended `buffersize`
		if (readSize == -1) {
			perror(NULL);
			ret = EXIT_FAILURE;
			break;
		}else if (readSize >= previousBuffersize) {
			// Input was too large
			fprintf(stderr, "Input is too large.\n");
			// Since getline updates buffer and buffer size,
			// revert them to their original states
			buffersize = previousBuffersize;
			inputBuffer = realloc(inputBuffer, buffersize);
			continue;
		}
		
		// Check if user wants to exit
		if(strcmp(inputBuffer, "\n") == 0) break;

		// Get operator by copying first element in input
		strncpy(operatorInput, inputBuffer, 1);
		operatorInput[1] = '\0';
		// Make sure it is either a plus or minus, any other char results in termination
		if (strcmp(operatorInput, "+") != 0 && strcmp(operatorInput, "-") != 0) {
			printf("Invalid operator.\n");
			printf("Usage: client operand(+|-) operator1 operator2\n");
			continue;
		}

		// Get first operand
		int firstSize = strcpyterm(operandOne, &inputBuffer[2], ' ', buffersize);
		if (!isValidNum(operandOne)) {
			printf("First operand invalid.\n");
			printf("Usage: client operand(+|-) operator1 operator2\n");
			continue;
		}

		// Get second operand
		strcpyterm(operandTwo, &inputBuffer[firstSize + 3], '\n', buffersize);
		if (!isValidNum(operandTwo)) {
			printf("Seconde operand invalid.\n");
			printf("Usage: client operand(+|-) operator1 operator2\n");
			continue;
		}

		// Create pipe to allow server to transmit result
		int fd[2];
		pid_t ch;

		if(pipe(fd) < 0) {
			perror("pipe");
			ret = EXIT_FAILURE;
			break;
		}

		ch = fork();
		if(ch < 0) {
			perror("fork");
			ret = EXIT_FAILURE;
			break;
		}else if (ch == 0) {
			// close input side of pipe
			close(fd[0]);
			// close stdout since it is not needed either
			close(1);
			// use pipe output instead
			dup(fd[1]);
			// invoke server and it should print to fd[1]
			execl("./server", "server", operatorInput, operandOne, operandTwo, 0);
			// display error and exit with failure
			perror("exec");
			ret = EXIT_FAILURE;
			break;
		}else {
			// wait for child
			wait(NULL);
			// close output side of pipe
			close(fd[1]);
			// read from fd[0]
			char str[64];
			// read from pipe
			read(fd[0], str, sizeof(str));
			// print result
			printf("%s\n", str);
		}
	}

	printf("Exiting program.\n");
	free(operatorInput);
	free(resultbuffer);
	free(operandOne);
	free(operandTwo);
	return ret;
}

int strcpyterm(char* dest, const char* src, char term, int destMax) {
	int count = 0;
	char curr;
	while(--destMax && (curr = *src) && curr != term) {
		*dest = curr;
		++dest;
		++src;
		++count;
	}
	*(++dest) = '\0';
	return count;
}

int isValidNum(char* str) {
	char* origstr = str;
	if(*str == '-' || *str == '+') ++str;
	while(*str) {
		if(*str < '0' || *str > '9') return 0;
		++str;
	}
	return 1;
}

