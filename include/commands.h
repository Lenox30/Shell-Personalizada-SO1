/**
 * @file commands.h
 * @brief Implementación de las funciones para analizar y ejecutar comandos en el shell.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "globals.h"
#include <fcntl.h>  // Add this line to include the definition of O_CREAT and other file control options
#include <signal.h> // Add this line to include the definition of SIGCONT
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // Add this line to include the definition of S_IRUSR and S_IWUSR
#include <sys/wait.h> // Add this line to include the definition of WUNTRACED
#include <unistd.h>

/**
 * @brief Analiza y ejecuta un comando dado.
 *
 * Esta función toma un comando en forma de cadena, lo analiza y ejecuta la acción correspondiente.
 * Soporta comandos internos como "cd", "clr", "echo", "quit", "fg", "bg", "start_monitor", "stop_monitor",
 * "status_monitor" y "update_config". Si el comando no es reconocido como un comando interno, se intenta
 * ejecutar como un programa externo.
 *
 * @param comando La cadena de caracteres que contiene el comando a analizar.
 * @return int Retorna 0 si el comando fue procesado correctamente.
 */
int analizar_comando(char*);

/**
 * @brief Maneja el comando 'cd' para cambiar el directorio de trabajo actual.
 *
 * Esta función cambia el directorio de trabajo actual basado en el argumento proporcionado.
 * Si no se proporciona argumento, imprime el directorio actual.
 * Si el argumento es "-", cambia al último directorio.
 * De lo contrario, cambia al directorio especificado.
 *
 * @param argumento El directorio al que se desea cambiar. Si es NULL o vacío, imprime el directorio actual.
 */
void Ctrl_CD(char*);

/**
 * @brief Cambia el directorio de trabajo actual al anterior.
 *
 * Esta función obtiene el valor de la variable de entorno OLDPWD, que almacena
 * el directorio de trabajo anterior. Si OLDPWD está definido, imprime la ruta
 * del directorio e intenta cambiar el directorio de trabajo actual al anterior.
 * Si la operación de cambio de directorio falla, imprime un mensaje de error.
 * Si OLDPWD no está definido, imprime un mensaje de error en stderr.
 */
void cambiar_a_oldpwd(void);

/**
 * @brief Cambia el directorio de trabajo actual al especificado.
 *
 * Esta función intenta cambiar el directorio de trabajo actual al directorio
 * especificado por el parámetro `directorio`. Si no se puede cambiar el
 * directorio, se muestra un mensaje de error.
 *
 * @param directorio Una cadena de caracteres que representa la ruta del
 * directorio al cual se desea cambiar.
 */
void cambiar_a_directorio(const char*);

/**
 * @brief Actualiza las variables de entorno del directorio de trabajo actual (PWD) y el directorio de trabajo anterior
 * (OLDPWD).
 *
 * Esta función guarda el directorio de trabajo actual en la variable de entorno `OLDPWD`,
 * luego actualiza la variable de entorno `PWD` al nuevo directorio de trabajo actual.
 * Si no se puede obtener el directorio de trabajo actual, se imprime un mensaje de error.
 *
 * @note Esta función asume que `cwd` es una variable global de tipo `char[]` con suficiente tamaño para contener la
 * ruta.
 */
void actualizar_pwd(void);

/**
 * @brief Limpia la pantalla del terminal.
 *
 * Esta función intenta limpiar la pantalla del terminal invocando el
 * comando "clear" usando la función system(). Si el comando falla,
 * se imprime un mensaje de error usando perror().
 */
void limpiar_pantalla(void);

/**
 * @brief Ejecuta el comando echo con soporte para redirección y variables de entorno.
 *
 * Esta función toma un argumento de cadena, lo tokeniza y procesa cada token.
 * Si se encuentra una redirección de entrada o salida, se maneja adecuadamente.
 * Además, si un token comienza con '$', se interpreta como una variable de entorno
 * y se imprime su valor. Si la variable no existe, se imprime un espacio.
 * Finalmente, se imprimen todos los tokens con un espacio entre ellos y una nueva línea al final.
 *
 * @param argumento La cadena de texto que contiene el comando y sus argumentos.
 */
void ejecutar_echo(char*);

/**
 * @brief Ejecuta un programa externo dado un comando.
 *
 * Esta función toma un comando como entrada, lo tokeniza en una lista de argumentos,
 * y luego crea un proceso hijo para ejecutar el programa especificado.
 * Soporta la ejecución en segundo plano si el comando termina con '&'.
 *
 * @param comando El comando a ejecutar, incluyendo sus argumentos.
 *
 * La función realiza las siguientes acciones:
 * - Tokeniza el comando en una lista de argumentos.
 * - Verifica si el comando debe ejecutarse en segundo plano.
 * - Crea un proceso hijo usando fork().
 * - En el proceso hijo, establece el grupo de procesos y maneja redirecciones antes de ejecutar el programa.
 * - En el proceso padre, maneja la ejecución en primer o segundo plano, y actualiza la lista de trabajos en segundo
 * plano.
 *
 * @note Si el comando está vacío, la función retorna sin hacer nada.
 * @note Si ocurre un error al crear el proceso hijo o al ejecutar el programa, se imprime un mensaje de error.
 */
void ejecutar_programa_externo(char*);

/**
 * @brief Maneja el comando 'fg' para reanudar un trabajo en primer plano.
 *
 * Esta función toma un número de trabajo, lo convierte en un PID y envía
 * una señal SIGCONT para reanudar el proceso en primer plano. Luego espera
 * a que el proceso termine o sea suspendido.
 *
 * @param job_number El número del trabajo que se desea reanudar.
 *
 * @note Si el PID no es válido, se imprime un mensaje de error.
 * @note Si ocurre un error al enviar la señal SIGCONT, se imprime un mensaje de error.
 */
void manejar_comando_fg(int);

/**
 * @brief Maneja el comando para reanudar un trabajo en segundo plano.
 *
 * Esta función toma un número de trabajo, lo convierte en un PID y envía
 * la señal SIGCONT para reanudar el proceso en segundo plano. Si el PID
 * no es válido, imprime un mensaje de error. Si la señal SIGCONT se envía
 * correctamente, el proceso se agrega a la lista de trabajos en segundo plano.
 *
 * @param job_number El número del trabajo que se desea reanudar.
 */
void manejar_comando_bg(int);

/**
 * @brief Ejecutar un comando con pipes
 *
 * Esta función toma una cadena de comando que puede contener múltiples comandos
 * separados por el carácter '|', los divide y los ejecuta en una secuencia de
 * procesos conectados por pipes.
 *
 * @param comando Una cadena de caracteres que contiene los comandos a ejecutar,
 * separados por el carácter '|'.
 *
 * La función realiza los siguientes pasos:
 * 1. Divide la cadena de comando en segmentos separados por '|'.
 * 2. Crea un pipe para conectar los procesos.
 * 3. Usa fork() para crear un nuevo proceso hijo para cada comando.
 * 4. Redirecciona la entrada y salida estándar según sea necesario para conectar
 *    los procesos a través del pipe.
 * 5. Ejecuta el comando actual en el proceso hijo.
 * 6. En el proceso padre, actualiza el descriptor de archivo de entrada para la
 *    próxima iteración.
 * 7. Espera a que todos los procesos hijos terminen antes de finalizar.
 *
 * @warning Si ocurre un error al crear el pipe o al hacer fork(), la función
 *          imprimirá un mensaje de error y terminará el proceso.
 */
void ejecutar_comando_con_pipes(char*);

/**
 * @brief Maneja las redirecciones de entrada y salida en un comando.
 *
 * Esta función recorre los argumentos de un comando en busca de operadores de redirección
 * de entrada ("<") y salida (">"). Si encuentra un operador de redirección, abre el archivo
 * correspondiente y redirige el flujo estándar (stdin o stdout) al archivo especificado.
 *
 * @param args Array de cadenas de caracteres que representan los argumentos del comando.
 *             El array debe estar terminado en NULL.
 *
 * @note Esta función modifica el array de argumentos para eliminar los operadores de redirección
 *       y los nombres de archivo correspondientes.
 *
 * @warning Si no se puede abrir un archivo de redirección, se imprime un mensaje de error y
 *          la función retorna sin realizar ninguna redirección.
 */
void manejar_redirecciones(char**);

#endif // COMMANDS_H
