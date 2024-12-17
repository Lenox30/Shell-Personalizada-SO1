/**
 * @file monitor.c
 * @brief Implementación de las funciones para iniciar, detener y verificar el estado del proceso de monitoreo.
 */

#include "monitor.h"
#include "globals.h"
#include "shell_utils.h"
#include <cjson/cJSON.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Tamaño del array de métricas
 */
#define SIZE_METRICS 10 // Define the size of the metrics array

/**
 * @brief PID del proceso en primer plano
 */
pid_t monitor_pid = -1; // PID del proceso de monitoreo

// Inicializa el monitor
void start_monitor()
{
    if (monitor_pid > 0)
    {
        printf("El monitor ya está en ejecución con PID %d\n", monitor_pid);
        return;
    }

    // Crear un proceso hijo para el monitor
    monitor_pid = fork();
    if (monitor_pid == 0)
    {
        // Este es el proceso hijo
        execl("bin/metrics", "bin/metrics", NULL); // Ejecutar el programa de monitoreo
        perror("Error al iniciar el monitor");     // Imprimir un mensaje de error si execl() falla
        exit(EXIT_FAILURE);
    }
    else if (monitor_pid < 0)
    {
        perror("Error al crear el proceso de monitor");
    }
    else
    {
        printf("Monitor iniciado con PID %d\n", monitor_pid);
    }
}

// Detiene el monitor
void stop_monitor()
{
    if (monitor_pid <= 0)
    {
        printf("El monitor no está en ejecución\n");
        return;
    }

    if (kill(monitor_pid, SIGTERM) == 0) // Envia la señal SIGTERM al monitor
    {
        printf("Monitor detenido con éxito\n");
        monitor_pid = -1;
    }
    else
    {
        perror("Error al detener el monitor");
    }
}

// Muestra el estado del monitor
void status_monitor()
{
    if (monitor_pid > 0)
    {
        printf("Monitor en ejecución con PID %d\n", monitor_pid);
    }
    else
    {
        printf("El monitor no está en ejecución\n");
    }
}

// Maneja el comando de actualización
void handle_update_command(char* input)
{
    char* token = strtok(input, " "); // Tokeniza la cadena de entrada
    // Procesa el intervalo de muestreo
    token = strtok(NULL, " "); // Obtiene el siguiente token
    if (!token)
    {
        fprintf(stderr, "Intervalo no proporcionado.\n");
        return;
    }
    int interval = atoi(token); // Convierte el token a un entero
    // Procesa las métricas
    char* metrics[SIZE_METRICS]; // Array para almacenar las métricas
    int metric_count = 0;        // Contador de métricas
    while ((token = strtok(NULL, " ")) != NULL && metric_count < 10)
    {
        metrics[metric_count++] = token;
    }

    if (metric_count == 0)
    {
        fprintf(stderr, "No se proporcionaron métricas para monitorear.\n");
        return;
    }

    // Actualiza la configuración y notifica al programa de monitoreo
    update_config(interval, metrics, metric_count);
    if (monitor_pid > 0)
    {
        stop_monitor();  // Detiene el monitor
        start_monitor(); // Reinicia el monitor
    }
    else
    {
        printf("Monitor no está en ejecución\n");
    }
}

// Actualiza el archivo de configuración
void update_config(int interval, char** metrics, int metric_count)
{
    cJSON* root = cJSON_CreateObject(); // Crea un objeto JSON raíz
    if (!root)
    {
        fprintf(stderr, "No se pudo crear el objeto JSON.\n");
        return;
    }

    cJSON_AddNumberToObject(root, "sampling_interval", interval); // Agrega el intervalo de muestreo

    cJSON* metrics_array = cJSON_CreateArray(); // Crea un array JSON
    if (!metrics_array)
    {
        fprintf(stderr, "No se pudo crear el array JSON.\n");
        cJSON_Delete(root);
        return;
    }

    for (int i = 0; i < metric_count; i++) // Agrega las metricas al array
    {
        cJSON_AddItemToArray(metrics_array, cJSON_CreateString(metrics[i]));
    }

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
