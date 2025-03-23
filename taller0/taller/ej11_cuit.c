#include <stdio.h>
#include <stdlib.h>

int multiplos[] = {5, 4, 3, 2, 7, 6, 5, 4, 3, 2};

void verificar_cuit(char* cuit){
    int len = 0;
    while (cuit[len] != '\0'){
        len++;
    }
    if (len != 11){
        printf("El CUIT debe tener 11 caracteres\n");
        return;
    }

    char tipo_global[3] = {cuit[0], cuit[1], '\0'};
    char dni[9] = {cuit[2], cuit[3], cuit[4], cuit[5], cuit[6], cuit[7], cuit[8], cuit[9], '\0'};
    char digito_verificador = cuit[10];

    int resultado_algoritmo_verificacion = 0;
    for (int i = 0; i < 10; i++){
        // -'0' es para convertir el char a int (por codigo ascii :D)
        resultado_algoritmo_verificacion += (cuit[i] - '0') * multiplos[i];
    }

    resultado_algoritmo_verificacion %= 11;
    int digito_real = 11 - resultado_algoritmo_verificacion;

    if (digito_real == digito_verificador - '0'){
        printf("CUIT valido\n");
    } else {
        printf("El CUIT ingresado no es valido. Su informacion corregida es: %s-%s-%d\n", tipo_global, dni, digito_real);
        
    }
    
    

        
}


int main(int argc, char* argv[]){
    if (argc != 2){
        printf("Usage: %s <cuit>\n", argv[0]);
        return 1;
    }

    char* cuit = argv[1];
    verificar_cuit(cuit);

    return 0;
}