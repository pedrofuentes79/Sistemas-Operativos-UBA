#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
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

	// n pipes (por ser anillo) y uno mas para la comunicacion padre->start
	int pipes[n+1][2];
	for (int i = 0; i<n+1; i++){
		pipe(pipes[i]);
	}
	
	// lista de hijos
	pid_t *hijos = malloc(sizeof(*hijos) * n);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, c, start);
    
	for(int i = 0; i < n; i++){
		pid_t pid = fork();
		if(pid == 0){
			// cierro los pipes que no uso
			for (int j = 0; j<n+1; j++){
				if (i==j){
					close(pipes[j][READ]);
				} else if (j == i-1){
					close(pipes[j][WRITE]);
				}
				else if (i != start){
					close(pipes[j][WRITE]);
					close(pipes[j][READ]);
				}
			}
			int current_number;
			if (i == start){
				read(pipes[n][READ], &current_number, sizeof(int));
				secret_number = generate_random_number();
				printf("El numero secreto es... %d", secret_number);

				// inicio la comunicacion
				current_number++;
				write(pipes[i][WRITE], &current_number, sizeof(secret_number));
			}
			
			while(read(pipes[i-1][READ], current_number, sizeof(int)) == sizeof(int)){	
				if(i == start && current_number > secret_number){
					// le mando al padre por el pipe n que ya termine
					write(pipes[n][WRITE], current_number, sizeof(current_number));

					// cierro el pipe
					close(pipes[i][WRITE]);
					}
				else {
					current_number++;
					write(pipes[i][WRITE], current_number, sizeof(int));
				}
			}
			close(pipes[i-1][READ]);
			close(pipes[i][WRITE]);
			exit(EXIT_SUCCESS);
				
		} else {
			hijos[i] = pid;
		}
	}

	write(pipes[n][WRITE], &c, sizeof(c));
	int result;
	read(pipes[n][READ], &result, sizeof(result));
	printf("El resultado es %d\n", result);
    
}
