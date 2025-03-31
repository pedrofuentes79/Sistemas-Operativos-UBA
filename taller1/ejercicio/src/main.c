#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

int N;
int K;
int J;
int pids_muertos[10];
int total_muertos = 0;

void sigterm_handler(int signal){
	// sacar un numero al azar entre 0 y total_procesos - 1
	srand(time(NULL) ^ getpid());
	int numero_aleatorio = rand() % N;
	 
	if (numero_aleatorio == J){
		printf("Obtuve el numero maldito. Muero\n");
		exit(EXIT_SUCCESS);
	} else {
		printf("sobrevivo\n");
		return; // sobrevivo
	}
}

void sigchld_handler(int signal){
	int pid = wait(NULL);
	pids_muertos[total_muertos] = pid;
	total_muertos++;

}

void ejecutarHijo(){
	signal(SIGTERM, sigterm_handler);
	while(1){sleep(1);};
}

int main(int argc, char* argv[]) {
  	// recibir N, K y J como parametros
	if (argc != 4) {
		printf("Error: se requieren tres parametros\n");
		exit(EXIT_FAILURE);
	}

	// escribo los valores en las variables globales, accesibles por los hijos ;)
	N = atoi(argv[1]);
	K = atoi(argv[2]);
	J = atoi(argv[3]);
	
	
	// validar inputs
	if (J >= N || J < 0){
		printf("Error: J debe ser menor que N y mayor o igual a 0\n");
		exit(EXIT_FAILURE);
	}
	if (N >= 10){
		printf("Error: N debe ser menor que 10\n");
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, sigchld_handler);
	int hijos[N];

	// crear N procesos hijos, cada uno con su identificador
	for (int i = 0; i < N; i++){
		int pid = fork();
		if (pid == 0){
			ejecutarHijo();
		} else if (pid>0){
			hijos[i] = pid;
		}
	}

	for (int ronda = 0; ronda < K; ronda++){
		// enviar sigterm a los N procesos hijos
		for (int i = 0; i < N; i++){
			kill(hijos[i], SIGTERM);
			sleep(1);
		}
	}

	// waitear los procesos que murieron


	for(int identificador = 0; identificador<N; identificador++){
		int pid_actual = hijos[identificador];

		int esta_muerto = 0;
		for (int i = 0; i<total_muertos; i++){
			if (pids_muertos[i] == pid_actual){
				esta_muerto = 1;
				break;
			}
		}

		if (!esta_muerto){
			printf("El proceso con id: %d y PID: %d sobrevivio\n", identificador, pid_actual);
			kill(pid_actual, SIGKILL);
		}
	}
}
