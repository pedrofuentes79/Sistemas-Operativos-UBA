#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include "constants.h"
enum {READ, WRITE};
int status, pid, n, start, c, secret_number;

int generate_random_number(){
	return (rand() % 50);
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	n = atoi(argv[1]);
	c = atoi(argv[2]);
	start = atoi(argv[3]);
	int pipePadre[2];
	if (pipe(pipePadre) == -1) {
		perror("Error creating pipe");
		exit(EXIT_FAILURE);
	}

	// n pipes (por ser anillo) y uno mas para la comunicacion padre->start
	int pipes[n][2];
	for (int i = 0; i<n; i++){
		if (pipe(pipes[i]) == -1) {
			perror("Error creating pipe");
			exit(EXIT_FAILURE);
		}
	}
	
	// lista de hijos
	pid_t *hijos = malloc(sizeof(*hijos) * n);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, c, start);
    
	for(int i = 0; i < n; i++){
		pid_t pid = fork();
		if(pid == 0){
			// DECIDO LOS INDICES EN BASE A EN QUE PROCESO ESTOY	
			int read_index, write_index;
			if (i == 0){
				read_index = n-1;
				write_index = 0;
			}
			else {
				read_index = i-1;
				write_index = i;
			}
			if (i != start){
				close(pipePadre[READ]);
				close(pipePadre[WRITE]);
			}


			// cierro los pipes que no uso
			for (int j = 0; j<n+1; j++){
				if (j == read_index){
					close(pipes[j][WRITE]);
				} else if (j == write_index){
					close(pipes[j][READ]);
				} else{
					close(pipes[j][WRITE]);
					close(pipes[j][READ]);
				}

			}
			

			int current_number;
			if (i == start){
				ssize_t bytes_read = read(pipePadre[READ], &current_number, sizeof(int));
				if (bytes_read == -1) {
					perror("Error reading from pipe");
					exit(EXIT_FAILURE);
				}
				secret_number = generate_random_number();
				printf("El numero secreto es... %d\n", secret_number);

				// inicio la comunicacion
				current_number++;
				ssize_t bytes_written = write(pipes[i][WRITE], &current_number, sizeof(secret_number));
				if (bytes_written == -1) {
					perror("Error writing to pipe");
					exit(EXIT_FAILURE);
				}
			}
			
			ssize_t bytes_read;
			while((bytes_read = read(pipes[read_index][READ], &current_number, sizeof(int))) > 0){	
				printf("soy el proceso %d. Recibí %d\n", i, current_number);
				if(i == start && current_number >= secret_number){
					// le mando al padre por el pipe n que ya termine
					ssize_t bytes_written = write(pipePadre[WRITE], &current_number, sizeof(current_number));
					if (bytes_written == -1) {
						perror("Error writing to pipe");
						exit(EXIT_FAILURE);
					}
					printf("envie a padre resultado\n");
					// cierro mis pipes de escritura
					close(pipes[write_index][WRITE]);
					close(pipePadre[WRITE]);
					close(pipePadre[READ]);
					}
				else {
					current_number++;
					ssize_t bytes_written = write(pipes[write_index][WRITE], &current_number, sizeof(int));
					if (bytes_written == -1) {
						perror("Error writing to pipe");
						exit(EXIT_FAILURE);
					}
				}
			}
			if (bytes_read == -1) {
				perror("Error reading from pipe");
				exit(EXIT_FAILURE);
			}
			// cierro mis pipes que uso
			close(pipes[read_index][READ]); 
			close(pipes[write_index][WRITE]);
			exit(EXIT_SUCCESS);
				
		} else {
			hijos[i] = pid;
		}
	}

	for(int i=0; i<n; i++){
		close(pipes[i][READ]);
		close(pipes[i][WRITE]);
	}
	ssize_t bytes_written = write(pipePadre[WRITE], &c, sizeof(c)); //numero inicial
	if (bytes_written == -1) {
		perror("Error writing to pipe");
		exit(EXIT_FAILURE);
	}

	for(int i=0; i<n; i++){
		wait(NULL);
	}

	int result;
	ssize_t bytes_read = read(pipePadre[READ], &result, sizeof(result));
	if (bytes_read == -1) {
		perror("Error reading from pipe");
		exit(EXIT_FAILURE);
	}
	close(pipePadre[READ]);
	close(pipePadre[WRITE])	;

	printf("El resultado es %d\n", result);
	return 0;
    
}
