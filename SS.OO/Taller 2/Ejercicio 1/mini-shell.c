#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

enum PIPE_TYPE
{
	PT_READ = 0,
	PT_WRITE = 1
};

void run_child(char** args)
{
	execvp(args[0], args);

	fprintf(stderr, "Error al ejecutar %s", args[0]);
	exit(-1);
}

static int run(char ***progs, size_t count)
{	
	int r, status;

	//Reservo memoria para el arreglo de pids
	//TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * count);

	int(*pipes)[2] = malloc(sizeof(*pipes) * (count - 1));

	for(int i = 0; i < count; ++i)
	{
		if(i < count - 1)
			pipe(pipes[i]);

		__pid_t new_child_pid = fork();

		//Si somos el proceso padre...
		if(new_child_pid != 0)
		{
			children[i] = new_child_pid;
		}	
		//Si somos el hijo...
		else
		{
			//Si no somos el ultimo, entonces vamos a querer reemplazar std out por un pipe para comunicarnos con nuestro sucesor
			if(i < count - 1)
			{
				close(pipes[i][PT_READ]);
				dup2(pipes[i][PT_WRITE], STD_OUTPUT);
			}

			//Si no somos el primero vamos a querer recibir input del pipe que nos conecta con nuestro predecesor
			if(i > 0)
			{
				close(pipes[i-1][PT_WRITE]);
				dup2(pipes[i-1][PT_READ], STD_INPUT);
			}

			//Cerramos todos los pipes de childs anteriores salvo nuestro antecesor directo
			for(int j = 0; j < (i-1); ++j)
			{
				close(pipes[j][PT_READ]);
				close(pipes[j][PT_WRITE]);
			}

			run_child(progs[i]);
		}
	}

	for(size_t i = 0; i < (count - 1); ++i)
	{
		close(pipes[i][PT_READ]);
		close(pipes[i][PT_WRITE]);
	}

	//TODO: Pensar cuantos procesos necesito
	//TODO: Pensar cuantos pipes necesito.

	//TODO: Para cada proceso hijo:
			//1. Redireccionar los file descriptors adecuados al proceso
			//2. Ejecutar el programa correspondiente

	//Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);
	free(pipes);

	return r;
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}

	int programs_count;
	char*** programs_with_parameters = parse_input(argv, &programs_count);

	printf("programas: %d\n", programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}