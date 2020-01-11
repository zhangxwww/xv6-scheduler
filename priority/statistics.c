#include "types.h"
#include "user.h"
#include "param.h"
#include "traps.h"
#include "syscall.h"

//#include "statistics.h"
//extern int time_slot_count;
//extern int cpu_running_time_slot_count;

void
interrupt(int *p, int int_type)
{
  int res;
  asm("mov %%esp, %%ebx\n\t"
      "mov %3, %%esp\n\t"
      "int %2\n\t"
      "mov %%ebx, %%esp" :
      "=a" (res) :
      "a" (int_type), "n" (T_SYSCALL), "c" (p) :
      "ebx");
}



#define IO_BUSY_TASK_TIMES 100


void IO_BUSY_TASK(){
  changePriority(getpid(), 1);
	int i = 0;
	for(; i<IO_BUSY_TASK_TIMES; i++) {
		sleep(1);
	}
}

void CPU_BUSY_SHORT_TASK(){
	changePriority(getpid(), 1);
	double x = 0;
	for (int k = 0; k < 100; k++){
		for (double z = 0; z < 1000.0; z+= 0.1){
			x =  x + 3.14 * 89.64;   // useless calculations to consume CPU time
		}
	}
}

void CPU_BUSY_LONG_TASK(){
  changePriority(getpid(), 5);
	printf(1, "entering CPU busy long task..\n");
	double x = 0;
	for(int i=0; i < 10; i++){
		for (int k = 0; k < 100; k++){
			for (double z = 0; z < 1000.0; z+= 0.1){
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
//        time_slot_count = 0;
  //      cpu_running_time_slot_count = 0;
	init();
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
    int total = get_total_time_slot_count();
	int used = get_total_cpu_running_time_slot_count();
  	printf(1, "IO busy task: \n%d/%d ready_time  %d/%d run_time  %d/%d sleep_time  %d/%d cycling_time\n", result[0][0], count[0], result[0][1], count[0], result[0][2], count[0], result[0][0]+result[0][1]+result[0][2], count[0]);
  	printf(1, "CPU busy short task: \n%d/%d ready_time  %d/%d run_time  %d/%d sleep_time  %d/%d cycling_time\n", result[1][0], count[1], result[1][1], count[1], result[1][2], count[1], result[1][0]+result[1][1]+result[1][2], count[1]);
  	printf(1, "CPU busy long task: \n%d/%d ready_time  %d/%d run_time  %d/%d sleep_time  %d/%d cycling_time\n", result[2][0], count[2], result[2][1], count[2], result[2][2], count[2], result[2][0]+result[2][1]+result[2][2], count[2]);
    printf(1, "1. average cycling time:%d/%d\n",result[0][0]+result[0][1]+result[0][2]+result[1][0]+result[1][1]+result[1][2]+result[2][0]+result[2][1]+result[2][2], 3*n);
	printf(1, "2. cpu use ratio:%d/%d\n", used, total);
    printf(1, "3. cpu throughput rate:%d/%d\n", 3*n, total);
	exit();
}
