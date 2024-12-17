/**
 * @file globals.h
 * @brief Definiciones de variables globales
 *
 * Este archivo contiene las definiciones de las variables globales utilizadas en el programa.
 *
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#include <limits.h>    // Include this header for PATH_MAX
#include <sys/types.h> // Include this header for pid_t

/**
 *  @brief Maximo tamaño de la linea de comando
 */
#define MAX_LINE 1024

/**
 *  @brief Maximo tamaño de los nombres
 */
#define MAX_NAMES 256

/**
 *  @brief Maximo tamaño de los trabajos en segundo plano
 */
#define MAX_JOBS 100

/**
 *  @brief Intervalo de tiempo en seg
 */
#define intervalo 10

// Variables globales inicializadas en main.c

/**
 *  @brief Directorio actual
 */
extern char cwd[PATH_MAX];

/**
 *  @brief Bandera para salir del shell
 */
extern int EXIT;

// Variables para el manejo de trabajos en segundo plano
/**
 *  @brief ID del trabajo en segundo plano
 */
extern int job_id;

/**
 *  @brief Lista para almacenar los PIDs de los trabajos en segundo plano
 */
extern pid_t jobs[MAX_JOBS];

/**
 *  @brief PID del proceso en primer plano
 */
extern pid_t proceso_en_primer_plano;

// Variables para el manejo de la terminal

/**
 *  @brief ID del grupo de procesos de la shell
 */
extern pid_t shell_pgid;

/**
 *  @brief Atributos de la terminal
 */
extern struct termios shell_tmodes;

/**
 *  @brief Terminal de la shell
 */
extern int shell_terminal;

/**
 *  @brief Bandera para verificar si la shell es interactiva
 */
extern int shell_is_interactive;

// Variables para el monitoreo de procesos
/**
 *  @brief PID del proceso de monitoreo
 */
extern pid_t monitor_pid; // PID del proceso de monitoreo

/**
 *  @brief Bandera para verificar si el monitor está en ejecución
 */
extern int running; // Bandera para controlar la ejecución del programa

#endif // GLOBALS_H
