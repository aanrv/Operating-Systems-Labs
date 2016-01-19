/*
 * Varan Sharma
 * CSc 332 - Operating Systems
 * Project 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sem.h"

enum {TOBACCO, PAPER, MATCHES};

#define PERMS 0666  // create shared memory with read and write permissions

const int N = 10;	// number of times agent will place items
int* buffer;		// buffer to hold items

int smutex;	// semaphore for modifying buffer to keep access mutually exclusive
int sage;	// semaphore for agent
int stob;	// sempahore for smoker with tobacco
int spap;	// semaphore for smoker with paper
int smat;	// semaphore for smoker with matches

void getSemaphores(void);
void createSemaphores(void);

/* Agent will place items on table. */
void agent(void);
/* Smoker with tobacco will create cigarette. */
void smoker_t(void);
/* Smoker with paper will create cigaerette. */
void smoker_p(void);
/* Smoker with  matches will create cigarette. */
void smoker_m(void);

int main(void) {
	// create shared memory buffer with rw perms
	int shmid = shmget(1000, 1, IPC_CREAT | PERMS);
	if(shmid < 0) {
		fprintf(stderr, "Unable to create shared buffer.\n");
		exit(EXIT_FAILURE);
	}

	// allocate buffer
	if((buffer = (int*)shmat(shmid, (char*)0, 0)) == (int*)-1) {
		fprintf(stderr, "Unable to allocate.\n");
		exit(EXIT_FAILURE);
	}

	// use smget to get semaphores with read write permissions
	getSemaphores();
	// initialize semaphores
	createSemaphores();

	// create child processes
	int pid_tob = fork();
	if(pid_tob < 0) { perror("pid_tob"); exit(EXIT_FAILURE); }
	if(pid_tob == 0) {
		while(1) {
			// in smoker with tobacco
			P(stob);	// wait for signal
			P(smutex);	// wait to modify buffer
			smoker_t();	// roll up cigarette
			V(sage);	// signal agent
			V(smutex);	// signal that buffer can now be modified again
		}
	}else {
		int pid_pap = fork();
		if(pid_pap < 0) { perror("pid_pap"); exit(EXIT_FAILURE); }
		if(pid_pap == 0) {
			while(1) {
				// in smoker with paper
				P(spap);
				P(smutex);
				smoker_p();
				V(sage);
				V(smutex);
			}

		}else {
			int pid_mat = fork();
			if(pid_mat < 0) { perror("pid_mat"); exit(EXIT_FAILURE); }
			if(pid_mat == 0) {
				while(1) {
					// in smoker with matches
					P(smat);
					P(smutex);
					smoker_m();
					V(sage);
					V(smutex);
				}
			}else {
				// in agent
				int i;
				for(int i = 0; i < N; ++i) {
					P(smutex);	// wait for signal to modify buffer

					agent();	// place items on table
					
					// signal the smoker with the remaining item
					int supply = *buffer;
					if(supply == TOBACCO) V(stob);
					else if(supply == PAPER) V(spap);
					else V(smat);

					V(smutex);	// done modifying buffer
					P(sage);	// now wait while a smoker process loop finishes
					putchar('\n');
				}

				// infinite loop, so kill child processes on exit
				kill(pid_pap, SIGUSR1);
				kill(pid_tob, SIGUSR1);
				kill(pid_mat, SIGUSR1);
				exit(EXIT_SUCCESS);
			}
		}
	}
}

void getSemaphores(void) {
	// create semaphores
	if((smutex = semget(IPC_PRIVATE, 1, PERMS | IPC_CREAT)) == -1) {
		fprintf(stderr, "Unable to create mutex semaphore.\n");
		exit(EXIT_FAILURE);
	}
	if((sage = semget(IPC_PRIVATE, 1, PERMS | IPC_CREAT)) == -1) {
		fprintf(stderr, "Unable to create agent semaphore.\n");
		exit(EXIT_FAILURE);
	}
	if((stob = semget(IPC_PRIVATE, 1, PERMS | IPC_CREAT)) == -1) {
		fprintf(stderr, "Unable to create tobacco semaphore.\n");
		exit(EXIT_FAILURE);
	}
	if((spap = semget(IPC_PRIVATE, 1, PERMS | IPC_CREAT)) == -1) {
		fprintf(stderr, "Unable to create paper semaphore.\n");
		exit(EXIT_FAILURE);
	}
	if((smat = semget(IPC_PRIVATE, 1, PERMS | IPC_CREAT)) == -1) {
		fprintf(stderr, "Unable to create matches semaphore.\n");
		exit(EXIT_FAILURE);
	}
}

void createSemaphores(void) {
	sem_create(smutex, 1);
	sem_create(sage, 0);
	sem_create(stob, 0);
	sem_create(spap, 0);
	sem_create(smat, 0);

}

void agent(void) {
	int supply = rand() % 3;	// determine supplies
	*buffer = supply;			// store in buffer

	if(supply == TOBACCO) printf("> Agent places paper and matches.\n");
	else if(supply == PAPER) printf("> Agent places tobacco and matches.\n");
	else printf("> Agent places tobacco and paper.\n");
}

void smoker_t(void) {
	printf("< Smoker with tobacco takes supplies.\n");
}

void smoker_p(void) {
	printf("< Smoker with paper takes supplies.\n");
}

void smoker_m(void) {
	printf("< Smoker with matches takes supplies.\n");
}

