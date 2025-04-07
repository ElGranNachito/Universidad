#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"
#include <signal.h>

int (*pipes)[2];

int processCount;
int start;

enum PIPE_TYPE
{
	PT_READ = 0,
	PT_WRITE = 1
};

int generate_random_number(){
	return (rand() % 50);
}

void close_all_unused_pipes(int child_idx, int previous_pipe)
{
	for(int i = 1; i <= processCount; ++i)
	{
		if(i != previous_pipe)
			close(pipes[i][PT_READ]);

		if(i != child_idx)
			close(pipes[i][PT_WRITE]);
	}

	close(pipes[0][PT_WRITE]);
	close(pipes[processCount + 1][PT_READ]);

	//Si este no es el proceso principal entonces no necesitamos los pipes para comunicarnos con el proceso padre
	if(child_idx != start)
	{
		close(pipes[0][PT_READ]);
		close(pipes[processCount + 1][PT_WRITE]);
	}
}

int get_previous_pipe(int child_idx)
{
	int previousPipe = (child_idx + processCount - 1);

	return previousPipe > processCount ? previousPipe - processCount : previousPipe;
}

void run_child(int child_idx)
{
	int previous_pipe = get_previous_pipe(child_idx);

	close_all_unused_pipes(child_idx, previous_pipe);

	int current = 0;

	while(1)
	{
		if(read(pipes[previous_pipe][PT_READ], &current, sizeof(current)) == 0)
		{
			printf("Me mori %i\n", child_idx);
			break;
		}	

		printf("Soy proceso %d y recibi %d\n", child_idx, current);
		current++;
		write(pipes[child_idx][PT_WRITE], &current, sizeof(current));
	}

	close(pipes[child_idx][PT_WRITE]);

	exit(0);
}

void run_first_child(int child_idx)
{
	int current = 0;

	read(pipes[0][PT_READ], &current, sizeof(current));

	int secret = current + generate_random_number() + 1;

	printf("Soy el proceso inicial (%d). Mi secreto es: %d\n", child_idx, secret);

	int previous_pipe = get_previous_pipe(child_idx);

	close_all_unused_pipes(child_idx, previous_pipe);

	while (current < secret)
	{
		printf("Soy proceso %d y recibi %d\n", child_idx, current);
		current++;
		write(pipes[child_idx][PT_WRITE], &current, sizeof(current));
		read(pipes[previous_pipe][PT_READ], &current, sizeof(current));
	}

	printf("Me mori %i\n", child_idx);

	close(pipes[child_idx][PT_WRITE]);

	write(pipes[processCount + 1][PT_WRITE], &current, sizeof(current));

	exit(0);
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, buffer;
	processCount = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	__pid_t* child_processes;

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", processCount, buffer, start);
    
	pipes 			= malloc(sizeof(*pipes) * (processCount + 2));
	child_processes = malloc(sizeof(*child_processes) * processCount);

	for(int i = 0; i <= processCount + 1; ++i)
		pipe(pipes[i]);

    for(int i = 0; i < processCount; ++i)
	{
		__pid_t new_child_id = fork();

		if(new_child_id == 0)
		{
			if(i + 1 == start)
				run_first_child(i + 1);
			else
				run_child(i + 1);

			exit(0);
		}
		else
		{
			child_processes[i] = new_child_id;
		}
	}

	//Cerramos todos los pipes que no vamos a necesitar
	for(int i = 1; i <= processCount; ++i)
	{
		close(pipes[i][PT_READ]);
		close(pipes[i][PT_WRITE]);
	}

	close(pipes[0][PT_READ]);
	close(pipes[processCount + 1][PT_WRITE]);

	write(pipes[0][PT_WRITE], &buffer, sizeof(buffer));

	int result = 0;

	read(pipes[processCount + 1][PT_READ], &result, sizeof(result));

	for(int i = 0; i < processCount; ++i)
		waitpid(child_processes[i], NULL, 0);

	printf("Terminaron todos. El resultado fue: %d\n", result);

	free(pipes);
	free(child_processes);

	exit(0);
}