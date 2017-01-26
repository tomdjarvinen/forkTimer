/*
 * forkTimer.c
 * Author: Thomas Jarvinen
 * main: estimates how long a fork takes by measuring a test repeatedly
 */


#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
/*
 * Method Name: main
 * Author: Thomas Jarvinen
 * Description: Method to measure how long a fork takes.  This program executes repeatedly, and records time of day (to accuracy of microseconds) before and after the fork
 * Arguments: None
 */
int main(void)
{
    int pipefd[2];								//Establish pipe for communication between parent and child
    pid_t cpid;									//This will be used to identify childs and parent processes
    int bufSec;									//Will hold elapsed time in seconds as a tempory value
    int buf_uSec;								//Will hold elapsed time in usecs as a temporary value
    int time[1000];								//Will hold our final list of times
	struct timeval t1;							//Timeval is the needed argument for the gettimeofday() method.  This will store the time prior to forking  
	struct timeval t2;  						//This will store the time after forking    
	if (pipe(pipefd) == -1) {					//check if pipe was created successfully
        perror("pipe");							//if not, record error
        exit(EXIT_FAILURE);						//Exit with a failure due to pipe error
    }
	
	int i;										//Dummy counter for loop
	
	for(i = 0; i< 1000; i = i + 1)				//this loop repeatedly executes a fork timing mechanism
	{
		gettimeofday(&t1,NULL);					//get starting time
		cpid = fork();							//Execute fork
		gettimeofday(&t2,NULL);					//Get time after the fork.  Both parent and child will take this measurement
		bufSec = t2.tv_sec - t1.tv_sec;			//time elapsed between measurements (seconds)
		buf_uSec = t2.tv_usec - t1.tv_usec;		//time elapsed between measurements (micro seconds)
		time[i] = buf_uSec+1000000*bufSec;		//Net time elapsed
		if (cpid == -1) {						//Check if fork was successful
			perror("fork");						//Record that fork through an error (i.e returned -1
			exit(EXIT_FAILURE);					//Exit with a failure due to fork 
		}
		if (cpid == 0) 							/* Child writes to pipe */
		{    
			write(pipefd[1], &(time[i]), 1);	//write to pipe
			close(pipefd[1]);         			// Reader will see EOF
			exit(EXIT_SUCCESS);							//Exit the program (also reaps children)
								//Exit loop, the child needs to be reaped to prevent "fork bomb"
		}
			else {            					//Parent reads from pipe
			read(pipefd[0], &bufSec, 1);		//Read value in pipe
			if(bufSec < time[i])				//Check whether parent or child ran first.  This is necessary, because it is up to the scheduler to determine what goes first
				time[i]=bufSec;					//If yes, replace the value in our array
			printf("%d\n",time[i]);	//Write result to terminal.  This is not the only way to record result, but it was chosen for simplicity.
		}
	}
	exit(EXIT_SUCCESS);							//Exit the program (also reaps children)
}
			
