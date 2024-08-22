// ---- EJERCICIO 1.2.2 ----
// Cristian Gustavo Mogensen
//
// Escriba un programa que verifique el directorio de trabajo actual (current working directory).
// Si el directorio de trabajo actual esta dentro del directorio HOME del usuario (USERNAME),
// deberá retornar un valor de retorno que indique la correcta finalización (0).
// Si el directorio de trabajo actual no esta dentro del directorio HOME del usuario (USERNAME),
// deberá retornar un valor de retorno que indique la finalización con error (1).
// Ejecute el programa por la linea de comandos y compruebe el valor de retorno para cada variante.

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int codigo_salida;

    // Se busca el valor de la variable de entorno DATE_FORMAT.
    char* pwd = getenv("PWD");
    char* home = getenv("HOME");

    if ((pwd == NULL) && (home == NULL)) {
        codigo_salida = 2;
    } else if ((pwd != NULL) && (home == NULL)) {
        codigo_salida = 3;
    } else if ((pwd == NULL) && (home != NULL)) {
        codigo_salida = 4;
    } else {
        codigo_salida = (pwd == home) ? 0 : 1;
    }
    
    // Mensajes de error.
    switch (codigo_salida) {
        case 2:
            printf("Variables de entorno HOME y PWD no encontradas.");
            break;
        
        case 3:
            printf("Variable de entorno HOME no encontrada.");
            break;

        case 4:
            printf("Variable de entorno PWD no encontrada.");
            break;

        default:
            break;
    }

    return codigo_salida;
}
