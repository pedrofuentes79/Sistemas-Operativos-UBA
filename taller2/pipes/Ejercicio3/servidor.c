#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/wait.h>

int MAX_CONEXIONES = 5;


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

void manejar_cliente(int client_socket) {
    char expresion[100];
    int resultado;
    
    recv(client_socket, expresion, sizeof(expresion), 0);
    printf("Servidor: recibí la expresión '%s' del cliente\n", expresion);
    
    resultado = calcular(expresion);
    printf("Servidor: el resultado es %d\n", resultado);
    
    send(client_socket, &resultado, sizeof(resultado), 0);
    close(client_socket);
}

void handler_sigchld(int sig) {
    // Cuando recibo un sigchld, es porque un cliente (hijo) terminó
    // Luego, lo waiteo para que no se quede colgado
    wait(NULL);
}

int main() {
    int server_socket;
    int client_socket;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    socklen_t server_len = sizeof(server_addr);
    socklen_t client_len = sizeof(client_addr);
    pid_t pid;

    // SIGCHLD: Cuando un hijo termina, lo waiteo
    signal(SIGCHLD, handler_sigchld);

    // Configuración del socket del servidor
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");
    unlink(server_addr.sun_path);

    // Crear socket, vincular y escuchar conexiones
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, server_len);
    listen(server_socket, MAX_CONEXIONES);

    printf("Servidor: esperando conexiones de clientes...\n");
    
    while(1) {
        // aceptar conexion del cliente
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_len);
        pid = fork();
        
        if (pid < 0) {
            // error al crear el proceso hijo
            perror("Error en fork");
            close(client_socket);
        } else if (pid == 0) {
            // codigo del proceso hijo
            // El hijo no necesita el socket del servidor, lo cierro
            close(server_socket); 
            manejar_cliente(client_socket);
            exit(0); 
        } else {
            // codigo del proceso padre
            printf("Servidor: creado proceso hijo %d para manejar al cliente\n", pid);
            // El padre no necesita el socket del cliente, lo cierro
            close(client_socket);  
        }
    }

    exit(0);
}

