#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Will read the grades from the file: filename and store into grades array.
// Returns 0 upon failure
int readGrades(char* filename, long** grades, long hwsPerLine);

// Given a 2d array of grades (of exactly 10 students), prints average for each hw
// Returns 0 upon failure
int printAverages(long** grades, long numChapters, long hwsPerChap);

// Copies a word from source to destination (stops at whitespace)
// Returns number of characters copied (without null byte) and appends null byte
int copyword(char* destination, char* source);

const size_t NUM_STUDENTS = 10;

int main(int argc, char** argv) {
	if(argc != 4) {
		fprintf(stderr, "Usage: director filename numChapters homeworksPerChapter\n");
		return EXIT_FAILURE;
	}

	char* filename = argv[1];
	long numChapters = strtol(argv[2], NULL, 10);
	if(numChapters <= 1) { fprintf(stderr, "Chapters must be >= 1\n"); return EXIT_FAILURE; }
	long hwsPerChap = strtol(argv[3], NULL, 10);
	if(hwsPerChap < 1) { fprintf(stderr, "Each chapter must have at least 1 homework\n"); return EXIT_FAILURE; }
	long hwsPerLine = numChapters * hwsPerChap;

	// array to hold grades
	long* grades[NUM_STUDENTS];	// A 2d array to hold grades
	int i;
	for(i = 0; i < NUM_STUDENTS; ++i) grades[i] = malloc(sizeof(long) * hwsPerLine);
	
	// return flag
	int out = EXIT_SUCCESS;
	// Read grades from file at filename and store them into grades
	if(!readGrades(filename, grades, hwsPerLine)) {
		fprintf(stderr, "Unable to read grades\n");
		goto failed;
	}
	
	// Print the averages for each homework
	if(!printAverages(grades, numChapters, hwsPerChap)) {
		fprintf(stderr, "Unable to print averages\n");
		goto failed;
	}
	// no issues, do not exit with failure
	goto succeeded;
failed:
	out = EXIT_FAILURE;
succeeded:
	for(i = 0; i < NUM_STUDENTS; ++i) free(grades[i]);
	return out;
}

int readGrades(char* filename, long** grades, long hwsPerLine) {
	// open file for reading
	FILE* f = fopen(filename, "r");
	if(f == NULL) {
		perror("input file");
		return 0;
	}

	// create buffer to hold line
	size_t buffsize = 128;
	char* linebuffer = malloc(buffsize);	// will temporarily hold lines from file
	char numberBuffer[4];			// will be used to iterate through each grade

	// iterate through array to add grades line by line
	int i;
	for(i = 0; i < NUM_STUDENTS; ++i) {
		size_t linesize = getline(&linebuffer, &buffsize, f);

		char* it = linebuffer;
		int gradeNum;
		for(gradeNum = 0; gradeNum < hwsPerLine; ++gradeNum) {
			// ignore whitespace
			while (*it == ' ' || *it == '\t') ++it;
			// retrieve next word
			int numDigits = copyword(numberBuffer, it);
			// convert to 
			long numberRead = strtol(numberBuffer, NULL, 10);
			grades[i][gradeNum] = numberRead;
			// move it forward by the length of the last word
			it += numDigits;
		}
	}
	// done reading from file
	free(linebuffer);	// free buffer that was used to hold each line
	fclose(f);		// close file

	return 1;
}

int printAverages(long** grades, long numChapters, long hwsPerChap) {
	// Create manager processes
	int i;
	for(i = 0; i < numChapters; ++i) {
		pid_t mg = fork();
		if(mg < 0) {
			perror(NULL);
			break;
		}else if(mg == 0) {
			// in manager, create workers based on number of chapters per hw
			int ch;
			for(ch = 0; ch < hwsPerChap; ++ch) {
				pid_t wk = fork();
				if(wk < 0) {
					perror(NULL);
					break;
				}else if(wk == 0) {
					// in worker
					long avg = 0;
					// get and print average of homeworks
					int studentNumber;
					int homeworkNumber = (i * hwsPerChap) + ch;
					for(studentNumber = 0; studentNumber < NUM_STUDENTS; ++studentNumber) {
						avg += grades[studentNumber][homeworkNumber];
					}
					avg /= NUM_STUDENTS;
					printf("Average for homework %d: %ld\n", homeworkNumber, avg);
					// no more work to do for worker, exit process
					exit(0);
				}
			}
			// after creating all children, manager should wait for them to finish
			while(wait(NULL) != -1)
				;
			if(errno != ECHILD) perror(NULL);
			// done waiting for children, manager can now exit
			exit(0);
		}
	}

	// wait for all chilren before exiting function
	while(wait(NULL) != -1)
		;
	if(errno != ECHILD) perror(NULL);
	return 1;
}

int copyword(char* destination, char* source) {
	int count = 0;
	char curr;
	while((curr = *source) && curr != ' ' && curr != '\n' && curr != '\r' && curr != '\t' && curr != EOF) {
		*destination = curr;
		++destination;
		++source;
		++count;
	}
	*destination = '\0';
	return count;
}

