#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

enum { READ, WRITE };

static int run(char ***progs, size_t n)
{
	// n es el numero de programas
	int r, status;

	//Reservo memoria para el arreglo de pids
	//TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * n);

	// n-1 pipes para n procesos
	int pipes[n-1][2];
	for (int i = 0; i < n-1; i++) {
		if (pipe(pipes[i]) == -1) {
			perror("Error al crear el pipe :(");
			return -1;
		}
	}
	//TODO: Para cada proceso hijo:
			//1. Redireccionar los file descriptors adecuados al proceso
			//2. Ejecutar el programa correspondiente

	// hoja de ruta
	// crear el primer proceso. Redirigir su stdout al primer pipe
	// crear el segundo proceso. Redirigir su stdin al primer pipe. Redirigir su stdout al segundo pipe
	// crear el tercer proceso. Redirigir su stdin al segundo pipe.
	// repetir hasta el proceso n-1
	// al ultimo proceso, redirijo su stdin al anterior pipe. su stdout queda asi, quiero que quede en la terminal`

	for (int i = 0; i < n; i++){
		pid_t pid = fork();
		if (pid == -1) {
			perror("Error al crear el proceso :(");
			return -1;
		} else if (pid == 0){
			// Redirijo el stdin al pipe anterior, excepto para el primer proceso.
			if (i > 0) {
				dup2(pipes[i-1][READ], STDIN_FILENO);
			}
			
			// Redirijo el stdout al pipe siguiente, excepto para el ultimo proceso.
			if (i < n-1) {
				dup2(pipes[i][WRITE], STDOUT_FILENO);
			}
			
			// CIERRO LOS PIPES QUE NO VOY A USAR
			for(int j = 0; j < n; j++){
				if (i==j){
					close(pipes[j][READ]);
				} else if (j == i-1) {
					close(pipes[j][WRITE]);
				} else {
					close(pipes[j][READ]);
					close(pipes[j][WRITE]);
				}
			}

			// Ejecuto el programa
			execvp(progs[i][0], progs[i]);
		} else {
			// Si el pid es distinto de 0, estoy en el contexto del padre.
			// Entonces me guardo el pid del proceso hijo
			children[i] = pid;
		}
	}

	for(int i=0; i<n; i++){
		if (i<n-1){
			close(pipes[i][READ]);
			close(pipes[i][WRITE]);
		}

		waitpid(children[i], &status, 0);
		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso [%d] %d no terminó correctamente [%d]: ",
				i, (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}

	r = 0;
	free(children);

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

	// programs_with_parameters es un array de arrays de strings
	// cada subarray es un programa con sus parametros
	// por ejemplo:
	// programs_with_parameters[0] = ["ls", "-a"]
	// programs_with_parameters[1] = ["grep", "anillo"]
	char*** programs_with_parameters = parse_input(argv, &programs_count);
	printf("programs_count: %d\n", programs_count);
	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
