#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int server_socket;
    int client_socket;
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

    printf("Servidor: esperando conexión del cliente...\n");
    while(1) {
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);
        recv(client_socket, &num, sizeof(num), 0);
        printf("Servidor: recibí %d del cliente!\n", num);
        num *= 2;
        send(client_socket, &num, sizeof(num), 0);
        close(client_socket);
    }

    exit(0);
}
