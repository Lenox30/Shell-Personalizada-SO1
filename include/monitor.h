/**
 * @file monitor.h
 * @brief Declaración de las funciones para iniciar, detener y verificar el estado del proceso de monitoreo.
 */

#ifndef MONITOR_H
#define MONITOR_H

/**
 * @brief Inicia el proceso de monitoreo si no está ya en ejecución.
 *
 * Esta función verifica si el proceso de monitoreo ya está en ejecución comprobando la variable global `monitor_pid`.
 * Si el proceso de monitoreo ya está en ejecución, imprime un mensaje con el PID del monitor en ejecución.
 * Si el proceso de monitoreo no está en ejecución, crea un proceso hijo usando `fork()`.
 * El proceso hijo intenta ejecutar el programa "bin/metrics" usando `execl()`.
 * Si la llamada a `execl()` falla, se imprime un mensaje de error y el proceso hijo sale con un estado de fallo.
 * Si la llamada a `fork()` falla, se imprime un mensaje de error.
 * Si la llamada a `fork()` tiene éxito, se imprime un mensaje con el PID del nuevo proceso de monitoreo creado.
 */
void start_monitor(void);

/**
 * @brief Detiene el proceso de monitoreo si está en ejecución.
 *
 * Esta función verifica si el proceso de monitoreo está en ejecución comprobando
 * la variable global `monitor_pid`. Si el proceso de monitoreo está en ejecución,
 * envía una señal SIGTERM para terminarlo. Si la terminación es exitosa, actualiza
 * `monitor_pid` a -1 y muestra un mensaje de éxito. Si la terminación falla,
 * muestra un mensaje de error.
 *
 * @note La función asume que `monitor_pid` es una variable global que contiene
 * el ID del proceso de monitoreo.
 */
void stop_monitor(void);

/**
 * @brief Verifica el estado del monitor.
 *
 * Esta función imprime el estado del monitor en ejecución. Si el monitor
 * está en ejecución, se muestra su PID. De lo contrario, se indica que
 * el monitor no está en ejecución.
 */
void status_monitor(void);

/**
 * @brief Maneja el comando de actualización analizando la cadena de entrada, extrayendo el intervalo de muestreo y las
 * métricas, actualizando la configuración y reiniciando el proceso de monitoreo si está en ejecución.
 *
 * @param input Una cadena que contiene el comando de actualización con el intervalo de muestreo y las métricas.
 *
 * La cadena de entrada debe estar en el formato: "update_config interval metric1 metric2 ... metricN"
 * donde interval es el intervalo de muestreo en segundos y metric1, metric2, ..., metricN son las métricas a
 * monitorear.
 *
 * La función realiza los siguientes pasos:
 * 1. Analiza la cadena de entrada para extraer el intervalo de muestreo y las métricas.
 * 2. Valida el intervalo y las métricas extraídas.
 * 3. Actualiza la configuración con el nuevo intervalo y métricas.
 * 4. Si el proceso de monitoreo está en ejecución, detiene y reinicia el monitor para aplicar la nueva configuración.
 *
 * Si no se proporciona el intervalo o no se especifican métricas, la función imprime un mensaje de error y retorna.
 *
 * Ejemplo de uso:
 * update_config 5 cpu_usage memory_usage
 */
void handle_update_command(char*);

/**
 * @brief Actualiza el archivo de configuración con el intervalo de muestreo y las métricas proporcionadas.
 *
 * Esta función crea un objeto JSON que contiene el intervalo de muestreo y un array de métricas,
 * luego escribe este objeto JSON en un archivo de configuración llamado "config.json".
 *
 * @param interval El intervalo de muestreo que se agregará a la configuración.
 * @param metrics Un array de cadenas que contiene las métricas a agregar.
 * @param metric_count El número de métricas en el array.
 */
void update_config(int, char**, int);

#endif // MONITOR_H
