#include <stdio.h>
#include <unistd.h>

int main(void) {
	int a = 10;
	int b = 25;
	int fq = 0;
	int fr = 0;
	
	fq = fork();
	if(fq == 0) {
		// In Q
		a = a + b;
		printf("a: %d, b: %d, pid: %d\n", a, b, getpid());
		fr = fork();
		if(fr != 0) {
			// In Q
			b = b + 20;
			printf("a: %d, b: %d, pid: %d\n", a, b, getpid());
		}else {
			// In R
			a = (a * b) + 30;
			printf("a: %d, b: %d, pid: %d\n", a, b, getpid());
		}
	}else {
		// In P
		b = a + b - 5;
		printf("a: %d, b: %d, pid: %d\n", a, b, getpid());
	}
}

