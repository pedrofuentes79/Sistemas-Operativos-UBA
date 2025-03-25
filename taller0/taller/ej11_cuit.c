#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "verificar_cuit.h"

int main(int argc, char* argv[]){
    if (argc != 2){
        printf("Usage: %s <cuit>\n", argv[0]);
        return 1;
    }

    char* cuit = argv[1];
    char* tipo_global = extraer_tipo_global(cuit);
    char* dni = extraer_dni(cuit);

    int digito_real = verificar_cuit(cuit);

    if (digito_real != -1){
        printf("El CUIT ingresado no es valido. Su informacion corregida es: %s-%s-%d\n", tipo_global, dni, digito_real);   
    } else {
        printf("CUIT valido\n");
    }

    free(tipo_global);
    free(dni);

    return 0;
}
