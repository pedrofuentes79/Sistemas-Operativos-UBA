#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void ejecutarHijo(int id, int total_procesos, int numero_maldito){

	// esperar a recibir sigterm
	// cuando la recibo
		// sacar un numero al azar entre 0 y total_procesos - 1
			// si el numero es el maldito; matarme (con FAILURE) y expresar mis ultimas palabras
			// en caso contrario sobrevivo
}


int main(int argc, char* argv[]) {
  	// recibir N, K y J como parametros
	if (argc != 4) {
		printf("Error: se requieren tres parametros\n");
		exit(EXIT_FAILURE);
	}

	int N = atoi(argv[1]);
	int K = atoi(argv[2]);
	int J = atoi(argv[3]);
	
	// validar inputs
	if (J >= N || J < 0){
		printf("Error: J debe ser menor que N y mayor o igual a 0\n");
		exit(EXIT_FAILURE);
	}
	if (N >= 10){
		printf("Error: N debe ser menor que 10\n");
		exit(EXIT_FAILURE);
	}

	pid_t hijos[N];

	// crear N procesos hijos, cada uno con su identificador
	for (int i = 0; i < N; i++){
		pid_t pid = fork();
		if (pid == 0){
			hijos[i] = pid;
			ejecutarHijo(i, N, J);
		}
	}

	for (int ronda = 0; ronda < K; ronda++){
		// enviar sigterm a los N procesos hijos
		for (int i = 0; i < N; i++){
			kill(hijos[i], SIGTERM);
			sleep(1);
		}
	}


	// A CHEQUEAR: 

	printf("\nHijos sobrevivientes:\n");
	for (int i = 0; i < N; i++){
		pid_t pid = wait(NULL);

	return 0;
}
