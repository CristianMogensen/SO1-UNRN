// ---- EJERCICIO 1.8 ----
// Cristian Gustavo Mogensen
// 
// Consigna:
// -----------
// Escriba un programa que realice las siguientes tareas:
// 
//   • Debe leer una lista de comandos por la entrada standar (Un comando por
//     linea).
//   • Debe ejecutar cada uno de los comandos, en un hilo independiente.
//   • Cada hilo debe imprimir por la salida standar el comando y el valor de
//     retorno, en formato: “timestamp” “comando” -> “retorno”.
//   • El formato del “timestamp”, le será indicado por parámetro -f. Si este
//     parámetro no se le indica, el formato por defecto es ISO8601
//     (https://es.wikipedia.org/wiki/ISO_8601).
//   • Además, si el valor de retorno es distinto de cero (0), debe imprimir
//     por la salida de errores el mensaje “El comando {comando}, fallo con
//     valor de retorno {retorno}”.
//   • El programa debe terminar, cuando no queden lineas para leer en la
//     entrada standar y todos los hilos que inició, hayan terminado.
//   • Además deberá registrar manejador de señal para TERM. Si se recibe la
//     señal, deberá dejar de leer la entrada standar y terminar, una vez hayan
//     terminado todo los hilos iniciados.
//   • Cuando el programa termine, deberá imprimir por la salida standar la
//     cantidad comandos ejecutados correctamente y la cantidad de comandos que
//     arrojaron error.
//   • El valor de retorno, debe ser igual a la cantidad de comandos que
//     arrojaron error.
//   • Realice pruebas sobre el programa, utilizando archivos y los operadores
//     de redirección de la linea de comandos. Proporcione el archivo de
//     comandos con el que realiza pruebas y el comando del interprete e
//     comandos que utilizó para probar en un archivo Readme.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
// gcc ejercicio-1.8.c -lpthread

#define MAX_CARACTERES 250 // cantidad máxima de caracteres al leer una línea
#define MAX_HILOS 20 // cantidad máxima de hilos, int mayor a cero

// Variables globales.
int   global_terminar = 0; // Le indica al programa si debe continuar o terminar.
char* global_formato_fecha = "YYYY-MM-DD THH-mm-SS"; // Formato que se imprimirá la fecha en el timestamp.
int   global_cantidad_exitos = 0; // Para imprimir la cantidad de comandos ejecutados correctamente y la
int   global_cantidad_errores = 0; // cantidad de comandos que arrojaron error.

// Para formato de timestamp (ejercicio 1.1.2).
int coincidencias_seguidas(char* texto, int posicion);
int reemplazar_numero(char* texto, int posicion_de_reemplazo, int numero_insertar);
int numero_digitos(char* texto, int numero, int digitos);
int validar_formato(char* texto_a_validar, char letra_reemplazar, int numero_insertar);
int validar_fecha(char* texto_a_fecha);

// Para hilos.
void* ejecutar_hilo(void* argumentos);
void imprimir_linea_log(char* timestamp, char* comando, int retorno);

// Para señales.
void handler_sigterm(int numero_signal);

int iniciar_main(int argc, char* argv[]);
void imprimir_msj_error_args_main();

int main(int argc, char* argv[]) {
    if (iniciar_main(argc, argv) != 0) return -1;
    
    // Maneja la señal TERM.
    signal(SIGTERM, &handler_sigterm);
    
    // Se crea una cantidad determinada de hilos al leer líneas. Se ejecutan, y
    // luego se vuelven a repetir el proceso.
    pthread_t hilo[MAX_HILOS];
    char linea[MAX_HILOS][MAX_CARACTERES];
    int hilos_necesarios;

    while (!global_terminar) {
        hilos_necesarios = 0;

        // Se leen los comandos mientras no se haya superado la cantidad de
        // hilos reservados en memoria, y mientras no se haya recibido la señal
        // de TERM.
        for (int i = 0; (i < MAX_HILOS) && (!global_terminar); i++) {
            if (fgets(linea[i], sizeof(linea[i]), stdin) == NULL) {
            //if (fscanf(stdin, "%s", linea[i]) == EOF) {
                // Si no hay más líneas, se termina el programa.
                global_terminar = 1;
            } else {
                linea[i][strcspn(linea[i], "\n")] = '\0'; // Reemplaza el '\n' con '\0'
                hilos_necesarios++;
            }
        }

        // Se crean los hilos necesarios y disponibles y se los ejecuta.
        for (int numero_hilo = 0; numero_hilo < hilos_necesarios; numero_hilo++) {
            pthread_create(&hilo[numero_hilo], NULL, ejecutar_hilo, (void *) &linea[numero_hilo]);
        }

        // Se unen los hilos para esperarlos a que terminen de ejecutarse.
        for (int numero_hilo = 0; numero_hilo < hilos_necesarios; numero_hilo++) {
            pthread_join(hilo[numero_hilo], NULL);
        }
    }

    // Se imprimen la cantidad de comandos ejecutados correctamente y la
    // cantidad de comandos que arrojaron error.
    fprintf(stdout, "\n");
    fprintf(stdout, "-------------------\n");
    fprintf(stdout, "Cantidad de comandos ejecutados correctamente: %d.\n", global_cantidad_exitos);
    fprintf(stdout, "Cantidad de comandos que arrojaron error: %d.\n", global_cantidad_errores);
    fprintf(stdout, "Fin del programa.\n");
    fprintf(stdout, "-------------------\n");

    return global_cantidad_errores;
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
    //itoa(numero, numero_char, 10);
    sprintf(numero_char, "%d", numero);

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



/**
 * @brief Ejecuta un hilo con el comando deseado.
 * 
 * @param argumentos void* comando que se desea ejecutar (se castea a char*)
 * @return void* 
 */
void* ejecutar_hilo(void* argumentos) {
    char* comando = (char*) argumentos;
    int   retorno_comando;
    char  salida_comando[128];

    // Ejecuta el comando iniciando un "pipe stream".
    FILE* fp = popen(comando, "r");
    
    if (fp == NULL) {
        fprintf(stderr, "Error: No se pudo intentar ejecutar el comando \"%s\".\n", comando);
        global_cantidad_errores++;
        
        // Sale del hilo.
        pthread_exit(NULL);
    } else {
        // En caso de que se haya podido intentar ejecutar el comando:
        // Lee la salida del comando ejecutado y la imprime.
        //while (fgets(salida_comando, sizeof(salida_comando), fp) != NULL) {
        //    printf("%s", salida_comando);
        //}
        // DESCOMENTAR EN CASO DE NECESITARLA.

        // Obtiene el código de salida o retorno del comando ejecutado.
        retorno_comando = pclose(fp);
    }

    if (retorno_comando != 0) {
        fprintf(stderr, "El comando \"%s\", fallo con valor de retorno %d\n", comando, retorno_comando);
        global_cantidad_errores++;
    } else {
        global_cantidad_exitos++;
    }

    // Imprime por la salida standar el comando y el valor de retorno, en
    // formato: “timestamp” “comando” -> “retorno”.
    char timestamp[100];
    strcpy(timestamp, global_formato_fecha);
    validar_fecha(timestamp);
    imprimir_linea_log(timestamp, comando, retorno_comando);

    // Sale del hilo.
    pthread_exit(NULL);
}

/**
 * @brief Imprime una línea del "log". El formato es:
 * “timestamp” “comando” -> “retorno”.
 * 
 * @param timestamp char*, formato de la fecha. Por ejemplo: "YYYY-MM-DD".
 * @param comando char* comando ejecutado.
 * @param retorno int retorno del comando ejecutado.
 */
void imprimir_linea_log (char* timestamp, char* comando, int retorno) {
    // Imprime por la salida standar el comando y el valor de retorno, en
    // formato: “timestamp” “comando” -> “retorno”.
    fprintf(stdout, "%s %s -> %d\n", timestamp, comando, retorno);
}



/**
 * @brief Maneja la señal TERM (SIGTERM). Imprime el mensaje de que lo recibió
 * junto con el código de la señal.
 * 
 * @param numero_signal int, código de la señal recibida.
 */
void handler_sigterm(int numero_signal) {
    fprintf(stdout, "\n");
    fprintf(stdout, "Se recibio señal TERM (SIGTERM).\n");
    fprintf(stdout, "Número de señal: %d.\n", numero_signal);
    fprintf(stdout, "Terminando...\n");
    
    // Le indica al programa que debe terminar.
    global_terminar = 1;
}



/**
 * @brief Imprime un mensaje de error en caso de que el usuario haya ejecutado
 * el programa con una cantidad incorrecta de argumentos, o con los argumentos
 * mal escritos.
 * 
 */
void imprimir_msj_error_args_main() {
    fprintf(stderr, "Se esperan recibir 2 argumentos o bien, ninguno por default.\n");
    fprintf(stderr, "Se espera que el primer argumento sea \"-f\" para indicar el formato de la fecha. ");
    fprintf(stderr, "O bien, se espera que no se ingrese y se utilizará el formato ISO8601.\n");
    fprintf(stderr, "Seguido de \"-f\" se indica el formato de la fecha.\n");
    fprintf(stderr, "Por ejemplo: -f YYYY-MM-DD.\n");
    fprintf(stderr, "Formato de fecha:\n");
    fprintf(stderr, "- Y: años\n");
    fprintf(stderr, "- M: meses\n");
    fprintf(stderr, "- D: días\n");
    fprintf(stderr, "- W: día de la semana\n");
    fprintf(stderr, "- H: horas\n");
    fprintf(stderr, "- m: minutos\n");
    fprintf(stderr, "- S: segundos\n\n");
}

/**
 * @brief Analiza los argumentos del main para cambiar el formato de la fecha
 * en el timestamp.
 * 
 */
int iniciar_main(int argc, char* argv[]) {
    // Analiza los argumentos recibidos al ejecutar el programa.
    if (argc != 1) {
        // Se indica formato de fecha.
        if (argc == 3) {
            if (!strcmp(argv[1], "-f")) {
                // Copia el formato de fecha especificado por el usuario.
                global_formato_fecha = malloc(100*sizeof(char));
                strcpy(global_formato_fecha, argv[2]);
                return 0;
            } else {
                imprimir_msj_error_args_main();
                return -1;
            }
        } else {
            imprimir_msj_error_args_main();
            return -1;
        }
    } else {
        // No se indica formato de fecha.
        return 0;
    }
}
