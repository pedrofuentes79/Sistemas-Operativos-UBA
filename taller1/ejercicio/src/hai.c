#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


int pid_padre;
int pid_hijo;

char* comando;
char** arreglo_params;

int contador_ya_vas = 0;

void sigurg_handler(int signal){
	contador_ya_vas++;
	printf("ya va!\n");

	if (contador_ya_vas == 5){
		kill(pid_padre, SIGINT);
		execvp(comando, arreglo_params);
	}
	return;
}

void sigint_handler(int signal){
	// espero a que mi hijo termine
	wait(NULL);
	exit(EXIT_SUCCESS);
}


void ejecutarHijo(){
	signal(SIGURG, sigurg_handler);
	while(1){};
}




int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <comando>\n", argv[0]);
        return 1;
    }

    comando = argv[1];

	int cantidad_chars = 0;
	for (int i = 2; i<argc; i++){
		cantidad_chars += strlen(argv[i]);
	}

	arreglo_params = (char**) malloc(argc * sizeof(char)* (cantidad_chars+1));
	int parametros_agregados = 0;
	for (int i=1; i<argc; i++){
		arreglo_params[parametros_agregados] = argv[i];
		parametros_agregados++;
	}
	arreglo_params[parametros_agregados] = NULL;
	parametros_agregados++;


	signal(SIGINT, sigint_handler);

	// ejecutar hijo, guardarme pid
	pid_padre = getpid();
	int pid = fork();
	if (pid == 0){
		ejecutarHijo();
	} else if (pid > 0) {
		pid_hijo = pid;
	}

	while(1){
		sleep(1);
		printf("sup!\n");
		kill(pid_hijo, SIGURG);
	}
}

