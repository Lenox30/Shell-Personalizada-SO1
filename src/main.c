/**
 * @file main.c
 * @brief Programa que simula un shell básico con funcionalidades limitadas
 *
 * Este programa implementa un shell básico que permite la ejecución de comandos
 * en modo interactivo y batch. Incluye funcionalidades para manejar trabajos en
 * segundo plano, monitoreo de procesos y su configuración inicial, entre otras cosas
 *
 * Autor: Lenox Graham
 * Fecha: 2024
 */

// Incluir bibliotecas necesarias
#include "commands.h"    // Incluir el archivo de funciones de comandos
#include "globals.h"     // Incluir el archivo de definiciones globales
#include "shell_utils.h" // Incluir el archivo de utilidades de shell
#include <stdio.h>       // Incluir la biblioteca estándar de entrada/salida
#include <termios.h>     // Incluir la biblioteca de control de terminal

/** @brief Punto de entrada principal para el programa shell.
 *
 * Este archivo contiene la función principal que inicializa el shell, carga la
 * configuración y procesa los comandos ya sea desde un archivo batch o desde stdin.
 *
 * El programa opera en dos modos:
 * - Modo interactivo: donde el usuario ingresa comandos a través de stdin.
 * - Modo batch: donde los comandos se leen desde un archivo especificado.
 *
 * El bucle principal lee comandos, los procesa y sale cuando se cumple una condición de salida.
 *
 * Funciones:
 * - inicializar_shell(): Inicializa el entorno del shell.
 * - load_config(): Carga la configuración predeterminada desde un archivo JSON.
 * - mostrar_prompt(): Muestra el prompt del shell.
 * - analizar_comando(const char* comando): Analiza y procesa el comando dado.
 *
 * @param argc Número de argumentos de la línea de comandos.
 * @param argv Array de argumentos de la línea de comandos.
 * @return int Estado de salida del programa.
 */
int main(int argc, char* argv[])
{
    inicializar_shell();         // Llamar a la función de inicialización al iniciar la shell
    load_config();               // Cargar la configuración predeterminada del archivo JSON
    char comando[MAX_LINE] = ""; // Buffer para almacenar el comando ingresado
    FILE* batch_file = NULL;     // Puntero al archivo de comandos

    // Verificar si se pasa un archivo de comandos como argumento
    if (argc == 2)
    {
        batch_file = fopen(argv[1], "r"); // Abrir el archivo en modo lectura
        if (batch_file == NULL)           // Manejar errores al abrir el archivo
        {
            perror("Error al abrir el archivo de comandos");
            return 1;
        }
    }

    // Bucle principal del shell: lee desde el archivo o stdin según corresponda
    while (EXIT)
    {
        // Si no estamos en modo batch, mostrar el prompt
        if (batch_file == NULL)
        {
            mostrar_prompt();
        }

        // Leer el comando desde el archivo o desde stdin
        if (batch_file)
        {
            if (fgets(comando, sizeof(comando), batch_file) == NULL)
            {
                break; // Salir si se alcanza el final del archivo
            }
        }
        else
        {
            if (fgets(comando, sizeof(comando), stdin) == NULL)
            {
                break; // Salir si se cierra stdin
            }
        }

        // Eliminar el salto de línea al final del comando
        comando[strcspn(comando, "\n")] = 0;

        // Analizar y procesar el comando
        if (analizar_comando(comando))
        {
            break;
        }
    }

    // Cerrar el archivo batch si se abrió
    if (batch_file != NULL)
    {
        fclose(batch_file);
    }

    printf("Saliendo del shell...\n");
    return 0;
}
