// ---- EJERCICIO 1.2.1 ----
// Cristian Gustavo Mogensen
//
// Escriba un programa que verifique la existencia de la variable de entorno DATE_FORMAT.
// Si la variable se encuentra definida, deberá imprimir el contenido por la salida standar, y retornar
// un valor de retorno que indique la correcta finalización (0).
// Si la variable no se encuentra definida, deberá imprimir un mensaje indicando la situación por la
// salida standar, y retornar un valor de retorno que indique la finalización con error (1).
// Ejecute el programa por la linea de comandos y compruebe el valor de retorno para cada variante.

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int codigo_salida;

    // Se busca el valor de la variable de entorno DATE_FORMAT.
    char* date_format = getenv("DATE_FORMAT");
    
    codigo_salida = (date_format == NULL) ? 1 : 0;

    if (codigo_salida == 1) {
        printf("Variable de entorno DATE_FORMAT no encontrada.");
    } else {
        printf(date_format);
    }

    return codigo_salida;
}
