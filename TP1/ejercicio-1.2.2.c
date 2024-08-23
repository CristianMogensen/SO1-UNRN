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
#include <string.h>

int comparar_directorios(char* path1, char* path2);

int main(int argc, char* argv[]) {
    int codigo_salida;

    // Se busca el valor de la variable de entorno DATE_FORMAT.
    char* pwd = getenv("PWD");
    char* home = getenv("HOME");
    printf("pwd: %s\n", pwd);
    printf("home: %s\n", home);

    if ((pwd == NULL) && (home == NULL)) {
        codigo_salida = 2;
    } else if ((pwd != NULL) && (home == NULL)) {
        codigo_salida = 3;
    } else if ((pwd == NULL) && (home != NULL)) {
        codigo_salida = 4;
    } else {
        codigo_salida = comparar_directorios(pwd, home) ? 0 : 1;
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

/**
 * @brief Compara dos directorios, en cadenas de caracteres. Tiene en cuenta
 * que pueden diferir las barras separadoras. Es decir, puede ser una división
 * '/' o '\'.
 * 
 * @param path1 char* cadena de caracteres a comparar, no null. Toma el final
 * de la cadena como un '\0'.
 * @param path2 char* cadena de caracteres a comparar, no null. Toma el final
 * de la cadena como un '\0'.
 * @return int 0 si son distintos, 1 si son iguales. -1 en caso de que path1
 * sea null, -2 en caso de que path2 lo sea.
 */
int comparar_directorios(char* path1, char* path2) {
    if (path1 == NULL) return -1;
    if (path2 == NULL) return -2;

    int resultado = 1;

    int i = 0;
    char car1 = *(path1);
    char car2 = *(path2);
    while ((car1 != '\0') && (car2 != '\0')) {
        if (car1 != car2) {
            if (!((car1 == '/') || (car1 == '\\')) || !((car2 == '/') || (car2 == '\\'))) {
                resultado = 0;
                break;
            }
        }
        printf("i:%04d\n", i);
        i++;
        car1 = *(path1 + i);
        car2 = *(path2 + i);
    }

    if ((car1 != '\0') || (car2 != '\0')) {
        resultado = 0;
    }

    return resultado;
}
