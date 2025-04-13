#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

int server_socket;


//Si se hace CTRL+C, le envía un exit al servidor para que termine el proceso del cliente
void handler_sigint(){
    write(server_socket,"exit",4);
    close(server_socket);
    exit(0);
}

int main() {

    struct sockaddr_un server_addr;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error");
        exit(1);
    }

    signal(SIGINT,handler_sigint);

    char expresion[100];
    int resultado;

    while(1){
        printf("Ingrese una expresión: ");
        scanf("%s", expresion);

        write(server_socket, expresion, sizeof(expresion));
        
        //Si se escribió exit ya no vamos a enviar más mensajes
        if (strcmp("exit",expresion) == 0){
            break;
        }

        read(server_socket, &resultado, sizeof(resultado));
        printf("Cliente: recibí %d del servidor!\n", resultado);
    }

    close(server_socket);
    exit(0);
}
