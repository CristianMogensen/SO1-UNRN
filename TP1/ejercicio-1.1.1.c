// ---- EJERCICIO 1.1.1 ----
// Cristian Gustavo Mogensen
//
// Escriba un programa que imprima por la salida standar el nombre del usuario y el nombre del
// host, en formato usuario@host.
// Para esto puede utilizar las variables de entorno USERNAME y HOSTNAME.
// Y alguna o ambas no se encuentran definidas, el programa deberá imprimir un mensaje por la salida
// standar indicando que variables no se encuetran definidas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int display_ayuda();
int display_error_args();
int display_nombres();

int main(int argc, char* argv[]) {
    int codigo_salida = 0;
    
    // Analiza los argumentos recibidos al ejecutar el programa.
    if (argc > 1) {
        if (!strcmp(argv[1], "-h")) {
            codigo_salida = display_ayuda();
        } else {
            codigo_salida = display_error_args();
        }
    } else {
        codigo_salida = display_nombres();
    }

    return codigo_salida;
}

/**
 * @brief Muestra mensaje de ayuda.
 * 
 * @return int 0
 */
int display_ayuda() {
    printf("Imprime por la salida standard el nombre del usuario y el nombre del host, en formato usuario@host.\n\n");
    printf("Argumentos esperados:\n");
    printf("\t-h : Muestra este mensaje de ayuda.\n\n");
    printf("Códigos de salida:\n");
    printf("\t- 0: Éxito.\n");
    printf("\t- 1: Se recibieron argumentos no esperados.\n");
    printf("\t- 2: La variable de entorno USERNAME no está definida.\n");
    printf("\t- 3: La variable de entorno HOSTNAME no está definida.\n");
    printf("\t- 4: Las variables de entorno USERNAME y HOSTNAME no están definidas.\n");
    return 0;
}

/**
 * @brief Muestra mensaje de error al recibir el primer argumento inesperado.
 * 
 * @return int 1
 */
int display_error_args() {
    printf("Error: Se recibieron argumentos no esperados.");
    return 1;
}

/**
 * @brief Muestra el nombre del usuario y el nombre del host, en formato usuario@host.
 * 
 * @return int.
 *          0: Ejecución exitosa.
 *          2: La variable de entorno USERNAME no está definida.
 *          3: La variable de entorno HOSTNAME no está definida.
 *          4: Las variables de entorno USERNAME y HOSTNAME no están definidas.
 */
int display_nombres() {
    char* username = getenv("USERNAME");
    char* hostname = getenv("HOSTNAME");
    int codigo_salida;

    if ((username == NULL) && (hostname == NULL)) {
        codigo_salida = 4;
        printf("Error: Las variables de entorno USERNAME y HOSTNAME no están definidas.");
    } else if ((username == NULL) && (hostname != NULL)) {
        codigo_salida = 2;
        printf("Error: La variable de entorno USERNAME no está definida.");
    } else if ((username != NULL) && (hostname == NULL)) {
        codigo_salida = 3;
        printf("Error: La variable de entorno HOSTNAME no está definida.");
    } else {
        codigo_salida = 0;
        printf("%s@%s", username, hostname);
    }

    return codigo_salida;
}
