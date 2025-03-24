#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ej11_cuit.h"

typedef struct {
    char* nombre;
    char* cuit;
    int edad;
} user_t;

user_t* usuarios[100];
int cantidad_usuarios = 0;

user_t* crearUsuario(char* nombre, char* cuit, int edad){
    int resultado = verificar_cuit(cuit);
    if (resultado != -1){
        return 0;
    }

    user_t* usuario = (user_t*)malloc(sizeof(user_t));
    usuario->nombre = nombre;
    usuario->cuit = cuit;
    usuario->edad = edad;

    usuarios[cantidad_usuarios] = usuario;
    cantidad_usuarios++;
    return usuario;
}


int agregarInformacionUsuario(){
    printf("Ingrese nombre de persona: ");
    char* nombre = (char*)malloc(sizeof(char) * 100);
    scanf("%s", nombre);

    printf("Ingrese edad de persona: ");
    int edad;
    scanf("%d", &edad);

    printf("Ingrese cuit");
    char* cuit = (char*)malloc(sizeof(char) * 100);
    scanf("%s", cuit);

    if(crearUsuario(nombre, cuit, edad) == 0){
        printf("La informacion ingresada no es valida\n");
        return 0;
    }
    else{
        printf("Su información fue cargada con exito!\n");
        return 1;
    }

}

void verInformacionUsuario(){

    if (cantidad_usuarios == 0){
        printf("No hay usuarios registrados\n");
        return;
    }

    for (int i = 0; i < cantidad_usuarios; i++){
        printf("nombre: %s, edad: %d, cuit: %s\n", usuarios[i]->nombre, usuarios[i]->edad, usuarios[i]->cuit);
    }
}


int main(){
    char comando[100];

    while(1){
        printf("> ");
        scanf("%s", comando);

        if (strcmp(comando, "verInformacionUsuario") == 0){
            verInformacionUsuario();
        }
        else if (strcmp(comando, "agregarInformacionUsuario") == 0){
            if (agregarInformacionUsuario() == 0){
                break;
            }
        }
        else {
            printf("Comando invalido\n");
        }
    }
    return 0;
}