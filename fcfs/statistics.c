#include "types.h"
#include "user.h"
#include "param.h"

#define IO_BUSY_TASK_TIMES 5

void IO_BUSY_TASK(){
	int i = 0;
	for(; i<IO_BUSY_TASK_TIMES; i++) {
		printf(1, "Testing IO_BUSY TASK %d", i);
	}
}

void CPU_BUSY_SHORT_TASK(){
	double x = 0;
	for (int k = 0; k < 100; k++){
		for (double z = 0; z < 10000.0; z+= 0.1){
			x =  x + 3.14 * 89.64;   // useless calculations to consume CPU time
		}
	}
}

void CPU_BUSY_LONG_TASK(){
	double x = 0;
	for(int i=0; i < 10; i++){
		for (int k = 0; k < 100; k++){
			for (double z = 0; z < 10000.0; z+= 0.1){
				x =  x + 3.14 * 89.64;   // useless calculations to consume CPU time
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	if (argc != 2){
		printf(1, "Usage: SMLsanity <n>\n");
		exit();
 	}
	int n;
	int retime;
	int rutime;
	int stime;
	n = atoi(argv[1]); // number of subprocesses used for statistics
	int result[3][3];
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			result[i][j] = 0;
		}
	}
	int pid;
	int count[3];
	for(int i = 0; i<3; i++){
		count[i]=0;
	}
	for (int i = 0; i < 3*n; i++){
		pid = fork();
		if (pid == 0) {
			int j = getpid()%3;
			if(j==0){
				IO_BUSY_TASK();
			}else if(j==1){
				CPU_BUSY_SHORT_TASK();
			}else if(j==2){
				CPU_BUSY_LONG_TASK();
			}
			exit(); // children exit here
		}
	}
	for (int i = 0; i < 3*n; i++) {
		pid = wait2(&retime, &rutime, &stime);
		int type=pid%3;
		count[type]+=1;
		result[type][0] += retime;
		result[type][1] += rutime;
		result[type][2] += stime;
	}
  	printf(1, "IO busy task: %d retime %d runtime %d stime %d count\n", result[0][0], result[0][1], result[0][2], count[0]);
  	printf(1, "CPU busy short task: %d retime %d runtime %d stime %d count\n", result[1][0], result[1][1], result[1][2], count[1]);
  	printf(1, "CPU busy long task: %d retime %d runtime %d stime %d count\n", result[2][0], result[2][1], result[2][2], count[2]);
	exit();
}
