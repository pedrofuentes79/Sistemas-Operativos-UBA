#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int multiplos[] = {5, 4, 3, 2, 7, 6, 5, 4, 3, 2};

int verificar_cuit(char* cuit){
    int len = 0;
    while (cuit[len] != '\0'){
        len++;
    }
    if (len != 11){
        printf("El CUIT debe tener 11 caracteres\n");
        return -1;
    }

    char digito_verificador = cuit[10];

    int resultado_algoritmo_verificacion = 0;
    for (int i = 0; i < 10; i++){
        // -'0' es para convertir el char a int (por codigo ascii :D)
        resultado_algoritmo_verificacion += (cuit[i] - '0') * multiplos[i];
    }

    resultado_algoritmo_verificacion %= 11;
    int digito_real = 11 - resultado_algoritmo_verificacion;

    if (digito_real == digito_verificador - '0'){
        return -1;
    } else {
        return digito_real;
    }
        
}

char* extraer_tipo_global(char* cuit){
    char* tipo_global = (char*)malloc(3 * sizeof(char));
    for(int i = 0; i < 2; i++) {
        tipo_global[i] = cuit[i];
    }
    tipo_global[2] = '\0';
    return tipo_global;
}

char* extraer_dni(char* cuit){
    char* dni = (char*)malloc(9 * sizeof(char));
    for(int i = 0; i < 8; i++) {
        dni[i] = cuit[i + 2];
    }
    dni[8] = '\0';
    return dni;
}

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