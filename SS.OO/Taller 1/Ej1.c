
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include<time.h>
#include <sys/wait.h>

int processCount;
int roundCount;
int cursedNumber;
int currentIdentifier = 0;

__pid_t childrenPIDs[10];
__pid_t deadChildrenPIDs[10];

void validate_argc(int argc)
{
    //Comparamos con 4 porque el primer arg es el nombre del binario
    if(argc != 4)
    {
        printf("%d\n",argc);
        errno = EXIT_FAILURE;
        perror("El numero de argumentos debe ser 3");

        exit(EXIT_FAILURE);
    }
}

void validate_and_parse_argv(char** argv)
{
    processCount    = atoi(argv[1]);
    roundCount      = atoi(argv[2]);
    cursedNumber    = atoi(argv[3]);

    if(processCount <= 0 || roundCount <= 0 || cursedNumber < 0 || cursedNumber >= processCount)
    {
        errno = EXIT_FAILURE;
        perror("Entrada invalida, todos los argumentos deben ser no negativos y el numero maldito debe ser menor al numero de procesos");

        exit(EXIT_FAILURE);
    }
}

void run_round()
{
    for(int i = 0; i < processCount; ++i)
    {
        kill(childrenPIDs[i], SIGTERM);

        sleep(1);
    }
}

int get_random_number()
{
    int result = rand() % processCount; 
    
    return result;
}

void term_handler_child(int sig)
{
    if(get_random_number() == cursedNumber)
    {
        printf("Puta carajo\n");

        kill(getppid(), SIGTERM);

        exit(0);
    }
}

void run_process()
{
    srand(getpid());

    signal(SIGTERM, term_handler_child);

    while(1)
        pause();
}

void create_child_processes()
{
    for(; currentIdentifier < processCount; ++currentIdentifier)
    {
        __pid_t newChildPID = fork();

        if(newChildPID == 0)
        {
            run_process();

            perror("Proceso hijo termino prematuramente");
            exit(EXIT_FAILURE);
        }
        else
        {
            childrenPIDs[currentIdentifier] = newChildPID;
        }
    }
}

void term_handler_parent(int sig)
{
    __pid_t deadChildPID = wait(NULL);

    for(int i = 0; i < processCount; ++i)
    {
        if(childrenPIDs[i] == deadChildPID)
        {
            deadChildrenPIDs[i] = deadChildPID;
        }
    }
}

int main(int argc, char** argv)
{
    validate_argc(argc);
    validate_and_parse_argv(argv);

    signal(SIGTERM, term_handler_parent);

    create_child_processes();

    for(; roundCount > 0; roundCount--)
        run_round();

    for(int i = 0; i < processCount; ++i)
    {
        if(deadChildrenPIDs[i] == childrenPIDs[i])
            continue;

        if(kill(childrenPIDs[i], SIGKILL) == 0)
        {
            wait(NULL);

            printf("Sobreviviente: Identificador: %d, PID: %d\n", i, childrenPIDs[i]);
        }
    }
}