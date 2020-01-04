#include "types.h"
#include "user.h"


int
main(int argc, char *argv[])
{
	if (argc != 2){
		printf(1, "Usage: SMLsanity <n>\n");
		exit();
 		}
	int i;
	int n;
	
	int k;
	int retime;
	int rutime;
	int stime;
	n = atoi(argv[1]); // number of subprocesses used for statistics
	int result[3];
	for(int i=0;i<3;i++){
		result[i] = 0;
	}
	int pid;
	for (i = 0; i < n; i++) {
		pid = fork();
		if (pid == 0) {//child
			double x = 0;
			for (k = 0; k < 100; k++){
				for (double z = 0; z < 10000.0; z+= 0.1){
					x =  x + 3.14 * 89.64;   // useless calculations to consume CPU time
				}
			}
			exit(); // children exit here
		}
		continue; // father continues to spawn the next child
	}
	for (i = 0; i < n; i++) {
		pid = wait2(&retime, &rutime, &stime);
		result[0] += retime;
		result[1] += rutime;
		result[2] += stime;
	}
	for (i = 0; i < 3; i++)
			result[i] /= n;
  	printf(1, "\nAverage ready time: %d\n", result[0]);
  	printf(1, "\nAverage running time: %d\n", result[1]);
  	printf(1, "\nAverage sleeping time: %d\n", result[2]);	exit();
}