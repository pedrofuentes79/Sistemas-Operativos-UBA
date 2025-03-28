#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define RANGO_MIN 2L
#define RANGO_MAX 1000000001L

// Constants 0 and 1 for READ and WRITE
enum { READ, WRITE };

int procesos;

int esPar(long numero) {
  return (numero & 1) == 0;
}

long contarPares(long desde, long hasta) {
  long cantidad = 0;
  for (long i = desde; i < hasta; ++i) {
    if (esPar(i)) {
      cantidad++;
    }
  }
  return cantidad;
}

void ejecutarHijo(int pipe_fd[]) {
  // Leer del i-ésimo pipe el rango [desde, hasta) para realizar el cómputo
  long desde;
  read(pipe_fd[READ], &desde, sizeof(desde));

  long hasta;
  read(pipe_fd[READ], &hasta, sizeof(hasta));

  // Contar pares y escribir el resultado
  long resultado = contarPares(desde, hasta);

  printf("Hijo %d: Contó %ld pares. Desde: %ld, hasta: %ld\n", getpid(), resultado, desde, hasta);
  write(pipe_fd[WRITE], &resultado, sizeof(resultado));
  exit(EXIT_SUCCESS);
}

int main(int argc, char const* argv[]) {
  // Parsear la cantidad de procesos
  if (argc != 2) {
    printf("Debe ejecutar con la cantidad de procesos N como parámetro.\n");
    printf("Ejemplo: %s N\n", argv[0]);
    return 1;
  }
  procesos = atoi(argv[1]);
  printf("procesos: %d\n", procesos);

  // Crear pipes
  int pipe_fd[procesos][2];
  for (int i = 0; i < procesos; i++) {
    pipe(pipe_fd[i]);
  }

  // Crear hijos
  for (int i = 0; i < procesos; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      ejecutarHijo(pipe_fd[i]);
    }
    else if (pid == -1) {
      perror("fork");
      exit(1);
    }
  }

  // Calcular rangos para cada hijo
  // El intervalo es: [RANGO_MIN, RANGO_MAX) (es decir, cerrado-abierto)
  long cantidad = ((RANGO_MAX - RANGO_MIN) + (procesos - 1)) / procesos;
  long desde = RANGO_MIN;
  for (int i = 0; i < procesos; i++) {
    long hasta = desde + cantidad;
    if (hasta > RANGO_MAX) hasta = RANGO_MAX;

    // Enviar rango a cada hijo
    // no me convendria primero escribir y luego crear los hijos??, 
    // asi no tienen que esperar a que yo escriba para empezar, arrancan directo
    write(pipe_fd[i][WRITE], &desde, sizeof(desde));
    write(pipe_fd[i][WRITE], &hasta, sizeof(hasta));
    desde = hasta;
  }

  // Cerrar pipes inteligentemente
  for (int i = 0; i < procesos; i++){
    close(pipe_fd[i][READ]);  
    close(pipe_fd[i][WRITE]); 
    wait(NULL); 
  }


  long resultado = 0;
  // Leer los resultados de cada hijo
  for (int i = 0; i < procesos; i++) {
    int resultado_hijo;
    read(pipe_fd[i][READ], &resultado_hijo, sizeof(resultado_hijo));
    resultado += resultado_hijo;
  }
  printf("Resultado total: %ld\n", resultado);

  return 0;
}
