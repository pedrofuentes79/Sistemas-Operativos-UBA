#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}

void correHijo(){
	while(1){}
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));


	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	// n*2 pipes para hacer el anillo + conexion entre los hijos con el padre 
	int pipes[n*2][2];
	
	// lista de hijos
	pid_t *hijos = malloc(sizeof(*hijos) * n);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
    
	for(int i = 0; i < n; i++){
		pid_t pid = fork();
		hijos[i] = pid;
		if(pid == 0){
			correHijo();
		}
		
	}
    /* COMPLETAR */
    
}
