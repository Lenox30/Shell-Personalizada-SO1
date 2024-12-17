/**
 * @file shell_utils.c
 * @brief Funciones auxiliares para la shell interactiva.
 */
#include "shell_utils.h"
#include "globals.h"
#include "monitor.h"
#include "signal_handlers.h"
#include <cjson/cJSON.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
// Variables globales

/**
 *  @brief Descriptor de archivo para el terminal
 */
int shell_terminal;

/**
 *  @brief Bandera para verificar si la shell es interactiva
 */
int shell_is_interactive;

/**
 *  @brief PID del grupo de procesos de la shell
 */
pid_t shell_pgid;

/**
 *  @brief Atributos de la terminal
 */
struct termios shell_tmodes;

// Configracion inicial de la terminal
void inicializar_shell()
{
    /* Verificar si estamos ejecutando interactivamente.  */
    shell_terminal = STDIN_FILENO;                 // Descriptor de archivo para el terminal
    shell_is_interactive = isatty(shell_terminal); // Verificar si la shell es interactiva

    if (shell_is_interactive) // Verificar si la shell es interactiva
    {
        /* Bucle hasta que estemos en primer plano.  */
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);

        // Ignorar las señales interactivas y de control de trabajos
        signal(SIGINT, manejador_senales);  // Permitir que el proceso en primer plano maneje SIGINT
        signal(SIGQUIT, manejador_senales); // Permitir que el proceso en primer plano maneje SIGQUIT
        signal(SIGTSTP, manejador_senales); // Permitir que el proceso en primer plano maneje SIGTSTP
        signal(SIGTTIN, SIG_IGN);           // Ignorar la señal de control de trabajos
        signal(SIGTTOU, SIG_IGN);           // Ignorar la señal de control de trabajos
        signal(SIGCHLD, manejador_SIGCHLD); // Manejar la señal SIGCHLD para procesos hijos
        signal(SIGTERM, handle_sigterm);    // Manejar la señal SIGTERM para detener el programa

        /* Ponernos en nuestro propio grupo de procesos.  */
        shell_pgid = getpid();                   // Obtener el PID del proceso
        if (setpgid(shell_pgid, shell_pgid) < 0) // Establecer el grupo de procesos
        {
            perror("No se pudo poner el shell en su propio grupo de procesos");
            exit(1);
        }

        /* Tomar control del terminal.  */
        tcsetpgrp(shell_terminal, shell_pgid);

        /* Guardar los atributos de terminal predeterminados para el shell.  */
        tcgetattr(shell_terminal, &shell_tmodes);
    }
}

// Cargar la configuración desde un archivo JSON y guardarla en un archivo
void load_config()
{
    cJSON* root = cJSON_CreateObject(); // Crea un objeto JSON raíz
    if (!root)
    {
        fprintf(stderr, "No se pudo crear el objeto JSON.\n");
        return;
    }

    cJSON_AddNumberToObject(root, "sampling_interval", intervalo); // Agrega el intervalo de muestreo

    cJSON* metrics_array = cJSON_CreateArray(); // Crea un array JSON
    if (!metrics_array)
    {
        fprintf(stderr, "No se pudo crear el array JSON.\n");
        cJSON_Delete(root);
        return;
    }

    cJSON_AddItemToArray(metrics_array, cJSON_CreateString("cpu_usage"));     // Agrega las métricas al array
    cJSON_AddItemToArray(metrics_array, cJSON_CreateString("memory_usage"));  // Agrega las métricas al array
    cJSON_AddItemToArray(metrics_array, cJSON_CreateString("network_usage")); // Agrega las métricas al array

    cJSON_AddItemToObject(root, "metrics", metrics_array); // Agrega el array de métricas al objeto

    char* json_string = cJSON_Print(root); // Convierte el objeto JSON a una cadena
    if (!json_string)
    {
        fprintf(stderr, "No se pudo imprimir el objeto JSON.\n");
        cJSON_Delete(root);
        return;
    }

    FILE* file = fopen("../config.json", "w"); // Abre el archivo de configuración para escritura
    if (!file)
    {
        perror("No se pudo abrir el archivo de configuración para escribir");
        cJSON_free(json_string);
        cJSON_Delete(root);
        return;
    }

    fprintf(file, "%s\n", json_string); // Escribe la cadena JSON en el archivo
    fclose(file);                       // Cierra el archivo

    cJSON_free(json_string); // Libera la cadena JSON
    cJSON_Delete(root);      // Libera el objeto JSON
}

// Mostrar el prompt de la shell
void mostrar_prompt()
{
    char usuario[MAX_NAMES];   // Variable para almacenar el nombre de usuario
    char hostname[MAX_NAMES];  // Variable para almacenar el nombre del host
    char prompt_dir[PATH_MAX]; // Variable para almacenar la ruta del prompt
    char home_dir[PATH_MAX];   // Variable para almacenar la ruta del home

    // Obtener el nombre de usuario
    char* user_env = getenv("USER");
    if (user_env != NULL)
    {
        strncpy(usuario, user_env, sizeof(usuario) - 1);
        usuario[sizeof(usuario) - 1] = '\0';
    }
    else
    {
        fprintf(stderr, "No se pudo obtener el nombre de usuario.\n");
        usuario[0] = '\0';
    }

    // Obtener el nombre del host
    gethostname(hostname, sizeof(hostname));

    // Obtener el directorio actual
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        // Obtener el directorio home del usuario
        snprintf(home_dir, sizeof(home_dir), "/home/%s", usuario);

        // Verificar si el directorio actual es el directorio home
        if (strcmp(cwd, home_dir) == 0)
        {
            printf("\033[1;33m%s@%s:\033[1;34m~\033[0m$ ", usuario,
                   hostname); // Mostrar "~" en lugar del directorio home
        }
        else if (strstr(cwd, home_dir) ==
                 cwd) // Verificar si el directorio actual es un subdirectorio del directorio home
        {
            snprintf(prompt_dir, sizeof(prompt_dir), "~%s",
                     cwd + strlen(home_dir)); // Mostrar "~" en lugar del directorio home
            printf("\033[1;33m%s@%s:\033[1;34m%s\033[0m$ ", usuario, hostname,
                   prompt_dir); // Mostrar el directorio relativo al home
        }
        else
        {
            printf("\033[1;33m%s@%s:\033[1;34m%s\033[0m$ ", usuario, hostname, cwd); // Mostrar el directorio completo
        }
    }
    else
    {
        perror("getcwd() error"); // Manejar errores al obtener el directorio
    }
}

// Liberar los recursos utilizados por la shell
void liberar_recursos()
{
    // Detener el monitor si está en ejecución
    if (monitor_pid > 0)
    {
        stop_monitor();
    }

    // Terminar todos los trabajos en segundo plano
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (jobs[i] != 0)
        {
            kill(jobs[i], SIGTERM);
            jobs[i] = 0;
        }
    }

    // Restaurar los atributos de la terminal
    if (shell_is_interactive)
    {
        tcsetattr(shell_terminal, TCSANOW, &shell_tmodes);
    }
}
