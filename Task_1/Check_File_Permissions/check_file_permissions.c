#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main (int argc, char* argv[])
{
	if(argc <= 1) {
		fprintf(stderr, "Usage: file1 [file 2 file 3 ...]\n");
		return EXIT_FAILURE;
	}
	
	// Loop through arguments
	int i;
	for(i = 1; i < argc; ++i) {
		char* filepath = argv[i];

		// Check if file exists
		if(access(filepath, F_OK) == 0) {
			printf("%s exists\n", filepath);
		}else {
			if(errno == ENOENT) fprintf(stderr, "%s does not exist\n", filepath);
			else if (errno == EACCES) printf("%s is not accessible\n", filepath);
			// Unable to access file
			// Continue to next file
			continue;
		}

		// Check for reading permissions
		if(access(filepath, R_OK) == 0) printf("%s has reading permissions\n", filepath);
		else printf("%s does not allow reading\n", filepath);

		// Check for writing permissions
		if(access(filepath, W_OK) == 0) printf("%s has writing permissions\n", filepath);
		else printf("%s does not allow writing\n", filepath);
		
		putchar('\n');
	}
	return EXIT_SUCCESS;
}

