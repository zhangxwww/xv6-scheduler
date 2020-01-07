#include "types.h"
#include "user.h"
#include "param.h"

#define IO_BUSY_TASK_TIMES 50000

void IO_BUSY_TASK(){
	int i = 0;
	for(; i<IO_BUSY_TASK_TIMES; i++) {
		printf(1, "Testing IO_BUSY TASK %d", i);
	}
}

void CPU_BUSY_SHORT_TASK(){
	double x = 0;
	for (k = 0; k < 100; k++){
		for (double z = 0; z < 10000.0; z+= 0.1){
			x =  x + 3.14 * 89.64;   // useless calculations to consume CPU time
		}
	}
}

void CPU_BUSY_LONG_TASK(){
	double x = 0;
	for(int i=0; i < 10; i++){
		for (k = 0; k < 100; k++){
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
	
	int k;
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
	int proc_type[NPROC];
	for(int i = 0; i<NPROC; i++){
		proc_type[i] = -1;
	}
	for (i = 0; i < n; i++) {
		for(int j = 0; j < 3; j++){
			pid = fork();
			if (pid == 0) {
				proc_type[getpid()]=j;
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
	}
	for (i = 0; i < 3*n; i++) {
		pid = wait2(&retime, &rutime, &stime);
		int type=proc_type[pid];
		if(type>0){
			result[type][0] += retime;
			result[type][1] += rutime;
			result[type][2] += stime;
		}
	}
	for (int i = 0; i < 3; i++)
		for(int j=0;j<3;j++){
			result[i][j] /= n;
		}
	}
  	printf(1, "IO busy task: %dretime %druntime %dstime\n", result[0][0], result[0][1], result[0][2]);
  	printf(1, "CPU busy short task: %dretime %druntime %dstime\n", result[1][0], result[1][1], result[1][2]);
  	printf(1, "CPU busy long task: %dretime %druntime %dstime\n", result[2][0], result[2][1], result[2][2]);
	exit();
}