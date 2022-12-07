#include <sys/types.h>
#include <string.h>
#include <sched.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ROW (100)
#define COL ROW
#define BILLION 1000000000L
#define NS_MS 1000000

int cpuid, count, i; 
long duration;
struct timespec begin;

struct sched_attr {
	uint32_t size;
	uint32_t sched_policy;
	uint64_t sched_flags;
	int32_t sched_nice;

	uint32_t sched_priority;

	uint64_t sched_runtime;
	uint64_t sched_deadline;
	uint64_t sched_period;
};


void keycontrol(int sig){
	struct timespec end;
	if(sig == SIGINT)

		printf("Done!! Process #%d : %06d %ldms\n", i, count, duration/1000000);

	exit(0);
}

int calc(int time, int cpu){
	int matrixA[ROW][COL];
	int matrixB[ROW][COL];
	int matrixC[ROW][COL];
	int i, j, k;
	
	cpuid = cpu;

	struct timespec beginTmp, endTmp;
	long msDiff = -1;
	int COUNT_100_MS = 0;  

	while(duration < time * BILLION) {
		clock_gettime(CLOCK_MONOTONIC, &beginTmp);
		for(i = 0; i < ROW; i++){
			for(j = 0; j < COL; j++){
				for(k = 0 ; k < COL; k++){
					matrixC[i][j] += matrixA[i][j] * matrixB[k][j];
				}
			}
		}

		count++;

		clock_gettime(CLOCK_MONOTONIC, &endTmp);
		msDiff = (BILLION * (endTmp.tv_sec - beginTmp.tv_sec) + (endTmp.tv_nsec - beginTmp.tv_nsec));
		
		duration += msDiff;
		if(duration / 10000000 > COUNT_100_MS){
			COUNT_100_MS = duration / 10000000; 
			printf("PROCESS #%02d count = %02d\n", cpuid, count);
		}
	}

	return 0;
}

int main(int argc, char* argv[]){
	// needed # of argument check	
	if(argc < 3 || argc > 3){
 		printf("Enter only 2 args!!\n");
		return -1;
	}

	struct sched_attr attr;
	memset(&attr, 0, sizeof(attr));

	attr.sched_priority = 10;
	attr.sched_policy = SCHED_RR;
	
	int res = syscall(SYS_sched_setattr, getpid(), &attr, 0);
	if (res == -1) printf("sched_setattr fail as %d\n", res);
	
	int processNum = atoi(argv[1]);
	int totalTime = atoi(argv[2]);

	for(i = 0; i < processNum; i++){
		pid_t pid = fork();
		
		// Error 
		if (pid < 0) exit(EXIT_FAILURE);
		
		// Child Process
		if (pid == 0){
			// Get Start Time
			printf("Creating Process: #%d\n", i);

			signal(SIGINT, keycontrol);
			
			// Matrix Calculation
			calc(totalTime, i);	
			
			printf("Done!! Process #%d : %06d %ldms\n", i, count, duration/NS_MS);
			return 1;
		} 		

		// Parent Process
		else {
		}		
	}
	while(wait(NULL) > 0);
}

