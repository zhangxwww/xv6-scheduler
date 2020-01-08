// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char *argv[] = { "sh", 0 };

#define STATISTICS_N 8
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

void statistics(){
  int n=STATISTICS_N;
	int retime;
	int rutime;
	int stime;
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
}



int
main(void)
{
  int pid, wpid;

  changePriority(getpid(), 1);

  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  //statistics();

  for(;;){
    printf(1, "init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf(1, "init: fork failed\n");
      exit();
    }
    if(pid == 0){
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
  }
}
