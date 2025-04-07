#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int calcular(const char *expresion) {
    int num1, num2, resultado;
    char operador;

    // Usamos sscanf para extraer los dos números y el operador de la expresión
    if (sscanf(expresion, "%d%c%d", &num1, &operador, &num2) != 3) {
        printf("Formato incorrecto\n");
        return 0;  // En caso de error, retornamos 0.
    }

    // Realizamos la operación según el operador
    switch (operador) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                resultado = num1 / num2;
            } else {
                printf("Error: División por cero\n");
                return 0;  // Si hay división por cero, retornamos 0.
            }
            break;
        default:
            printf("Operador no reconocido\n");
            return 0;  // Si el operador no es válido, retornamos 0.
    }

    return resultado;
}

void handle_connection(int client_socket)
{
    printf("Nueva conexion aceptada\n");

    char received_command[512];

    while(1)
    {
        if(recv(client_socket, received_command, sizeof(received_command), 0) <= 0 || 
           strcmp(received_command, "exit\n") == 0)
            break;

        int result = calcular(received_command);

        send(client_socket, &result, sizeof(result), 0);
    }

    printf("Conexion con cliente finalizada\n");

    close(client_socket);

    exit(0);
}

int main() {
     
    // COMPLETAR. Este es un ejemplo de funcionamiento básico.
    // La expresión debe ser recibida como un mensaje del cliente hacia el servidor.
    const char *expresion = "10+5";  
    int resultado = calcular(expresion);
    printf("El resultado de la operación es: %d\n", resultado);

    int server_socket;

    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;

    uint slen = sizeof(server_addr);
    uint clen = sizeof(client_addr);
    int num;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");
    unlink(server_addr.sun_path);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, slen);
    listen(server_socket, 1);

    printf("Esperando conexiones\n");
    while(1) {
        int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);

        __pid_t new_child_pid = fork();

        if(new_child_pid == 0)
        {
            handle_connection(client_socket);
            exit(0);
        }
    }

    exit(0);
}

