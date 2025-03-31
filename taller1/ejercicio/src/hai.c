#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int pid_padre;
int pid_hijo;



int sigint_handler(int signal){
	// espero a que mi hijo termine
	wait(NULL);
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <comando>\n", argv[0]);
        return 1;
    }

    char* comando = argv[1];

	signal(SIGINT, sigint_handler)



	// ejecutar hijo, con un clone, guardarme pid

	for (int i=0; i<5;i++){
		printf("sup!\n");
		kill(pid_hijo, SIGURG);


	}




}
