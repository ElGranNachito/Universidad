#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

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
        perror("El numero de argumentos debe ser 3");

        exit(EXIT_FAILURE);
    }
}

void validate_and_parse_argv(char const* argv[])
{
    processCount    = atoi(argv[1]);
    roundCount      = atoi(argv[2]);
    cursedNumber    = atoi(argv[3]);

    if(processCount <= 0 || roundCount <= 0 || cursedNumber < 0 || cursedNumber >= processCount)
    {
        perror("Entrada invalida, todos los argumentos deben ser no negativos y el numero maldito debe ser menor al numero de procesos");

        exit(EXIT_FAILURE);
    }
}

void run_round()
{
    printf("ronda\n");
    for(int i = 0; i < processCount; ++i)
    {
        kill(childrenPIDs[i], SIGTERM);

        sleep(1);
    }
}

int generate_random_number(){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL)+getpid());
    return rand() % processCount;
}

void term_handler_child(int sig)
{
    int n = generate_random_number();

    if(n == cursedNumber)
    {
        printf("Puta carajo\n");

        kill(getppid(), SIGTERM);

        exit(0);
    }
}

void run_process()
{
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

int main(int argc, char const *argv[]){
    
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

    exit(EXIT_SUCCESS);
}
