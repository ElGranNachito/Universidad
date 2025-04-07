#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

int server_socket;

void terminate()
{
	close(server_socket);

	exit(0);
}

void sigint_handler(int sig)
{
	terminate();
}

int main() {
	
	signal(SIGINT, sigint_handler);
	
    struct sockaddr_un server_addr;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar con el servidor");
        exit(1);
    }

	printf("Establecida conexion con el servidor\n");

	char command[512];

	while(1)
	{
		int result;

		fgets(command, 511, stdin);

		write(server_socket, command, strlen(command) + 1);

		if(strcmp(command, "exit\n") != 0)
		{
			read(server_socket, &result, sizeof(result));

			printf("El resultado es: %d\n", result);
		}
		else
			break;
			
	}
	
	terminate();
}
