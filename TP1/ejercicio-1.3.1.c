// ---- EJERCICIO 1.3.1 ----
// Cristian Gustavo Mogensen
// 
// Escriba un programa que imprima por la salida standar, la fecha y hora actual.
// Por defecto, deberá imprimir la misma en formato ISO8601. (https://es.wikipedia.org/wiki/ISO_8601).
// Para modificar el formato de impresión, se podrá hacer por dos medios. A través de la variable de
// entorno DATE_FORMAT, o a través del argumento -f.
// El comportamiento debe ser el siguiente:
// 1. Si se especifica el parámetro -f, se debe usar el formato que este indica.
// 2. Si no se especifica -f, pero esta definida la variable DATE_FORMAT, se debe usar el formato
// indicado por la variable.
// 3. si ninguno de ambos se encuentra definido, se debe imprimir el formato ISO8601.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int coincidencias_seguidas(char* texto, int posicion);
int reemplazar_numero(char* texto, int posicion_de_reemplazo, int numero_insertar);
int numero_digitos(char* texto, int numero, int digitos);
int validar_formato(char* texto_a_validar, char letra_reemplazar, int numero_insertar);
int validar_fecha(char* texto_a_fecha);

int display_error_argc();
int display_error_arg_f();
int display_hora_f(char* texto_a_fecha);
int display_hora(char* texto_a_fecha);

int main(int argc, char* argv[]) {
    int codigo_salida;

    // Analiza los argumentos recibidos al ejecutar el programa.
    if (argc != 1) {
        if (argc == 3) {
            if (!strcmp(argv[1], "-f")) {
                char formato[100];
                strcpy(formato, argv[2]);
                codigo_salida = display_hora_f(formato);
            } else {
                codigo_salida = display_error_arg_f();
            }
        } else {
            codigo_salida = display_error_argc();
        }
    } else {
        char formato[] = "YYYY-MM-DDTHH-mm-SS";
        codigo_salida = display_hora(formato);
    }

    return codigo_salida;
}

/**
 * @brief Muestra el error que no se han insertado la cantidad de argumentos
 * esperados.
 * 
 * @return int 1
 */
int display_error_argc() {
    printf("Error: cantidad de argumentos inesperada.\n");
    printf("Se esperan 2 argumentos, \"-f\" seguido del DATE_FORMAT, o sino ningun argumento.\n");
    return 1;
}

/**
 * @brief Muestra el error que no se ha insertado el argumento -f al ingresar
 * argumentos.
 * 
 * @return int 2 
 */
int display_error_arg_f() {
    printf("Error: primer argumento inesperado.\n");
    printf("Se espera el argumento \"-f\" seguido del DATE_FORMAT.\n");
    return 2;
}

/**
 * @brief Muestra la fecha y hora en el formato especificado por el argumento
 * -f seguido del formato.
 * 
 * @param texto_a_fecha char* texto a validar fecha y hora actuales.
 * @return int código de salida de la función validar_fecha().
 */
int display_hora_f(char* texto_a_fecha) {
    int salida = validar_fecha(texto_a_fecha);
    printf("%s", texto_a_fecha);
    return salida;
}

/**
 * @brief Muestra la fecha y hora en el formato especificado por la variable de
 * entorno DATE_FORMAT o, por defecto en caso de que no esté definida, en el
 * formato especificado por el argumento de la función.
 * 
 * @param texto_a_fecha char* texto a validar fecha y hora actuales.
 * @return int código de salida de la función validar_fecha().
 */
int display_hora(char* texto_a_fecha) {
    // Se obtiene la variable de entorno DATE_FORMAT.
    char* date_format  = getenv("DATE_FORMAT");
    int   existe_var   = (date_format != NULL);
    int   salida;
    char resultado[100];

    if (existe_var) {
        strcpy(resultado, date_format);
    } else {
        strcpy(resultado, texto_a_fecha);
    }

    salida = validar_fecha(resultado);
    printf("%s", resultado);

    return salida;
}

/**
 * @brief Cuenta la cantidad de coincidencias seguidas que hay de un char
 * dentro de una cadena de char's. Esto es, por ejemplo, en una cadena "aaa"
 * desde la posición 0 contará 3 coincidencias. Para "aba" desde la posición 0
 * contará 1 coincidencia. O, por ejemplo, para "aaaa aaaa" desde la posición
 * 1, contará 3 coincidencias. No contará más que las coincidencias seguidas
 * de un caracter que haya en una posición determinada de la cadena.
 * 
 * @param texto char* texto a analizar, no null.
 * @param posicion int mayor a 0 (dentro del rango del texto).
 * @return int positivo con las coincidencias. -1 si el texto es null, -2 si la
 * posicion es menor a cero.
 */
int coincidencias_seguidas(char* texto, int posicion) {
    // Se verifican precondiciones.
    if (texto == NULL) return -1;
    if (posicion < 0) return -2;

    // Se busca la letra en la posicion indicada para buscar coincidencias
    // seguidas a partir de la posición.
    char letra_buscada = *(texto + posicion);
    int coincidencias = 0;
    for (int i = posicion; letra_buscada == *(texto + i); i++) {
        coincidencias ++;
    }

    return coincidencias;
}

/**
 * @brief Reemplaza un número en una cadena de caracteres en donde hayan
 * coincidencias de un mismo caracter. Esto es, por ejemplo, para la cadena
 * "YYYY" con posicion_de_remplazo 0 y numero_insertar 2024, el resultado será
 * "2024". Mientras que para, por ejemplo, "MM-YYYYYY-DD YY", con posición 3 y
 * el mismo numero_insertar, el resultado será "MM-002024-DD YY".
 * 
 * @param texto char* texto a tratar, no null.
 * @param posicion_de_reemplazo int posición no negativa en la que se quiere
 * insertar el número (reemplaza el caracter).
 * @param numero_insertar int numero a insertar en el texto.
 * @return int 0 si se ejecutó exitosamente, 1 si el texto es null, 2 si la
 * posicion_de_reemplazo no es válida. Códigos 10 correspondientes a los
 * errores de la función coincidencias_seguidas(). Códigos 20 correspondientes
 * a errores de la función numero_digitos().
 */
int reemplazar_numero(char* texto, int posicion_de_reemplazo, int numero_insertar) {
    // Se verifican precondiciones.
    if (texto == NULL) return 1;
    if (posicion_de_reemplazo < 0) return 2;

    // Se busca la cantidad de coincidencias para determinar el número de
    // dígitos que deben reemplazarse dentro de la cadena. Si hay problemas de
    // ejecución se retorna el código de error de la función + 10.
    char letra_buscada = *(texto + posicion_de_reemplazo);
    int digitos = coincidencias_seguidas(texto, posicion_de_reemplazo);
    if (coincidencias_seguidas <= 0) return abs(digitos) + 10;

    // Se convierte el número int a char. Si hay problemas de ejecución se
    // retorna el código de error de la función + 20.
    char numero_char[100];
    int numero_char_a_int = numero_digitos(numero_char, numero_insertar, digitos);
    if (numero_char_a_int != 0) return numero_char_a_int + 20;

    // Se reemplaza el número, con la cantidad de dígitos correspondiente.
    // Por ejemplo, si se quiere reemplazar "YY" por "2024", el resultado será
    // "24". Si se quiere reemplazar "YYYYYY" por el mismo valor, el resultado
    // será "002024".
    for (int i = 0; i < digitos; i++) {
        *(texto + posicion_de_reemplazo + i) = *(numero_char + i);
    }

    return 0;
}

/**
 * @brief Valida el formato de una cadena de caracteres. Se indica la letra
 * que se desea reemplazar por un número dentro del texto. Por ejemplo, para el
 * texto "YYYYYY YY" con el número 2024 y la letra 'Y', el resultado será
 * "002024 24".
 * 
 * @param texto_a_validar char* texto a validarle el formato [de fecha].
 * @param letra_reemplazar char letra a reemplazar por el número indicado.
 * @param numero_insertar int número a insertar en el texto.
 * @return int 0 si se ejecutó exitosamente, 1 si el texto_a_validar es null.
 * Códigos 10 corresponden a errores con la función reemplazar_numero().
 */
int validar_formato(char* texto_a_validar, char letra_reemplazar, int numero_insertar) {
    // Se verifican precondiciones.
    if (texto_a_validar == NULL) return 1;

    // Se busca la letra a reemplazar dentro del texto que se quiere validar.
    // Por ejemplo, la letra 'M' por el número del mes, dentro de un texto que
    // contenga "MM".
    int tamanio_texto = strlen(texto_a_validar);
    int posicion = 0;
    int digitos;
    int resultado;
    while (posicion < tamanio_texto) {
        if (*(texto_a_validar + posicion) == letra_reemplazar) {
            resultado = reemplazar_numero(texto_a_validar, posicion, numero_insertar);
            
            // Si hubo problemas en la ejecución, se retorna el código de error
            // de la función + 10.
            if (resultado != 0) return resultado + 10;
        } else {
            posicion ++;
        }
    }

    return 0;
}

/**
 * @brief Transforma un número en una cadena de caracteres, con la cantidad de
 * dígitos a mostrar. Por ejemplo, el número 2024, con 6 dígitos, sería
 * "002024". Mientras que el mismo número con 2 dígitos sería "24".
 * 
 * @param texto char* cadena de caracteres en el que se guardará el resultado.
 * La cadena de caracteres debe tener mínimo la cantidad de dígitos que se
 * especifica.
 * @param numero int positivo.
 * @param digitos int positivo (min 1, max 100), cantidad de digitos a mostrar de 'numero'.
 * @return int 0 si se ejecutó correctamente, 1 si el texto es NULL, 2 si el
 * numero no es positivo, 3 si la cantidad de digitos no es válida.
 */
int numero_digitos(char* texto, int numero, int digitos) {
    // Se verifican precondiciones.
    if (texto == NULL) return 1;
    if (numero <= 0) return 2;
    if ((digitos <= 0) || (digitos > 100)) return 3;

    // Transforma el número en cadena de caracteres.
    char numero_char[101];
    itoa(numero, numero_char, 10);

    int numero_tamanio = strlen(numero_char);
    if (digitos <= numero_tamanio) {
        // Se copian los numeros.
        for (int i = 0; i < digitos; i++) {
            *(texto + i) = numero_char[numero_tamanio - digitos + i];
        }
    } else {
        // (digitos > numero_tamanio)
        int i = 0;

        // Primero se agregan los ceros.
        for (; i < (digitos - numero_tamanio); i++) {
            *(texto + i) = '0';
        }

        // Luego se agrega el número.
        for (int j = 0; i < digitos; i++) {
            *(texto + i) = numero_char[j];
            j++;
        }
    }

    // Se agrega el final de la cadena.
    *(texto + digitos) = '\0';

    return 0;
}

/**
 * @brief Valida la fecha en un formato indicado. Inserta la fecha y/o hora
 * actual. Por ejemplo, para "YYYY-MM-DD" el resultado puede ser "2024-08-23".
 * El formato utilizado es el siguiente:
 * - Y: años - como 2024
 * - M: meses - como 8 (agosto)
 * - D: días - como 23
 * - W: día de la semana - como 5 (viernes)
 * - H: horas - como 15
 * - m: minutos - como 10
 * - S: segundos - como 30
 * 
 * @param texto_a_fecha char* texto a validar/insertarle la fecha actual.
 * @return int 0 si la ejecución fue exitosa, 1 si el texto_a_fecha es NULL.
 * Luego, 2 si hubo problemas con validar_formato para los años, 3 para los
 * meses, 4 para días, 5 para días de la semana, 6 para hora, 7 para minutos y
 * 8 para segundos.
 */
int validar_fecha(char* texto_a_fecha) {
    // Se verifican precondiciones.
    if (texto_a_fecha == NULL) return 1;

    // Se obtiene el valor del tiempo en el momento de ejecución.
    time_t tiempo_ahora         = time(0);
    struct tm* tiempo_resultado = localtime(&tiempo_ahora);

    int anio = (tiempo_resultado -> tm_year) + 1900;
    int mes  = (tiempo_resultado -> tm_mon) + 1;
    int dia  = (tiempo_resultado -> tm_mday);
    int sem  = (tiempo_resultado -> tm_wday);
    int hora = (tiempo_resultado -> tm_hour);
    int mins = (tiempo_resultado -> tm_min);
    int segs = (tiempo_resultado -> tm_sec);

    // Formato de fecha:
    // - Y: años
    // - M: meses
    // - D: días
    // - W: día de la semana
    // - H: horas
    // - m: minutos
    // - S: segundos
    char formato_anio = 'Y';
    char formato_mes  = 'M';
    char formato_dias = 'D';
    char formato_sem  = 'W';
    char formato_hora = 'H';
    char formato_mins = 'm';
    char formato_segs = 'S';

    // Se realiza la inserción de la fecha en el texto con el formato indicado.
    int validacion = validar_formato(texto_a_fecha, formato_anio, anio);
    if (validacion != 0) return 2;
    validacion = validar_formato(texto_a_fecha, formato_mes, mes);
    if (validacion != 0) return 3;
    validacion = validar_formato(texto_a_fecha, formato_dias, dia);
    if (validacion != 0) return 4;
    validacion = validar_formato(texto_a_fecha, formato_sem, sem);
    if (validacion != 0) return 5;
    validacion = validar_formato(texto_a_fecha, formato_hora, hora);
    if (validacion != 0) return 6;
    validacion = validar_formato(texto_a_fecha, formato_mins, mins);
    if (validacion != 0) return 7;
    validacion = validar_formato(texto_a_fecha, formato_segs, segs);
    if (validacion != 0) return 8;

    return 0;
}
