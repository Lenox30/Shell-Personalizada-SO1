/**
 * @file signal_handlers.c
 * @brief Implementación de los manejadores de señales para el shell.
 */

#include "signal_handlers.h"
#include "globals.h"
#include "shell_utils.h"
#include <stdio.h>
#include <sys/wait.h>
// Variables globales

/**
 * @brief Variable para controlar la ejecución del programa
 */
int running = 1;

// Manejador de señales
void manejador_senales(int sig)
{
    if (proceso_en_primer_plano != 0) // Verificar si hay un proceso en primer plano
    {
        kill(-proceso_en_primer_plano, sig); // Enviar la señal al proceso en primer plano
    }
    else
    {
        printf("\n");
        mostrar_prompt();
        fflush(stdout);
    }
}

// Manejador de señal para manejar procesos hijos
void manejador_SIGCHLD(int sig __attribute__((unused)))
{
    int status; // Variable para almacenar el estado del proceso
    pid_t pid;  // Variable para almacenar el PID del proceso hijo

    // Recolectar todos los procesos hijos que hayan terminado
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0; i < MAX_JOBS; i++) // Buscar el PID en la lista de trabajos
        {
            if (jobs[i] == pid) // Verificar si el proceso hijo es un trabajo en segundo plano
            {
                printf("\n[%d] Proceso %d terminado\n", i + 1, pid);
                job_id--;         // Decrementar el ID de trabajo
                jobs[i] = 0;      // Limpiar el PID de la lista de trabajos
                mostrar_prompt(); // Mostrar el prompt después de manejar SIGCHLD
                fflush(stdout);   // Vaciar la salida estándar
                break;
            }
        }
    }
}

// Manejador de señal para detener el programa
void handle_sigterm(int sig __attribute__((unused)))
{
    running = 0;
}
