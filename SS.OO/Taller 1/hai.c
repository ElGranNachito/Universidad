#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

__pid_t childPID;

int callCounter = 0;
char childExecutedCommand = 0;
char* command;
char** commandArgs;

void child_urg_handler()
{
	printf("ya va!\n");

	callCounter++;
}

void child_process()
{
	signal(SIGURG, child_urg_handler);

	while(callCounter < 5)
	{
		pause();
	}

	kill(getppid(), SIGINT);

	if(execvp(command, commandArgs) != 0)
	{
		printf("Todo mal");
	}
}

void parent_int_handler(int sig)
{
	childExecutedCommand = 1;
}

void parent_process()
{
	signal(SIGINT, parent_int_handler);

	sleep(1);

	while(!childExecutedCommand)
	{
		printf("sup!\n");

		kill(childPID, SIGURG);

		sleep(1);
	}

	wait(NULL);	
}

int main(int argc, char** argv) {
  	
	command 	= argv[1];
	commandArgs = &argv[1];
	
	childPID = fork();

	if(childPID == 0)
	{
		child_process();
	}
	else
	{
		parent_process();
	}

	return EXIT_SUCCESS;
}
