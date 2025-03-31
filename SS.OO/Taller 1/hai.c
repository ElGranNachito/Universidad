#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

__pid_t childPID;

void child_urg_handler()
{

}

void child_process()
{
	signal(SIGURG, child_urg_handler);
}

void parent_int_handler(int sig)
{
	sigset_t sigs2Block;
	sigset_t old;

	sigemptyset(&sigs2Block);
	sigemptyset(&sigs2Block);
	sigaddset(&sigs2Block, SIGCHLD);
	sigprocmask(SIG_BLOCK, &sigs2Block, &old);
	signal(SIGCHLD, NULL);
	sigprocmask(SIG_SETMASK, &old, NULL);
}

void parent_process()
{
	signal(SIGINT, parent_int_handler);
}

int main(int argc, char* argv[]) {
  	
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
