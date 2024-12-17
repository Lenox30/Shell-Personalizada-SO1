/**
 * @file test_shell.c
 * @brief Pruebas unitarias para el shell
 *
 * Para compilar y ejecutar las pruebas:
 * gcc tests/test_shell.c shell.c commands.c monitor.c signal_handlers.c -o test_shell -lcmocka
 * ./test_shell
 */

#include "commands.h"
#include "monitor.h"
#include "signal_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unity/unity.h>

#define MONITOR_OFF -1
// Prototipos de funciones

/**
 * @brief Prueba la función Ctrl_CD
 *
 * Esta función prueba la función Ctrl_CD con diferentes argumentos.
 */
void test_Ctrl_CD(void);
/**
 * @brief Prueba las funciones start_monitor y stop_monitor
 *
 * Esta función prueba las funciones start_monitor y stop_monitor.
 */
void test_start_monitor_and_stop_monitor(void);
/**
 * @brief Prueba la función ejecutar_comando_con_pipes
 *
 * Esta función prueba la función ejecutar_comando_con_pipes.
 */
void test_ejecutar_comando_con_pipes(void);

/**
 * @brief Prueba la función handle_sigterm
 *
 * Esta función prueba la función handle_sigterm.
 */
void test_handle_sigterm(void);

// Funciones de configuración y limpieza
void setUp(void)
{
    // Configuración antes de cada prueba
    monitor_pid = MONITOR_OFF; // Restablecer el PID del monitor
    // Configuración inicial: running debe estar activo (1)
    running = 1;
}
// Funciones de configuración y limpieza
void tearDown(void)
{
    // Limpieza después de cada prueba
}

/**
 * @brief Función principal de las pruebas
 *
 * Esta función llama a todas las pruebas unitarias.
 */
int main(void)
{
    UNITY_BEGIN(); // Inicia Unity

    // Llama a tus funciones de prueba
    RUN_TEST(test_Ctrl_CD);
    RUN_TEST(test_start_monitor_and_stop_monitor);
    RUN_TEST(test_ejecutar_comando_con_pipes);
    RUN_TEST(test_handle_sigterm);

    return UNITY_END(); // Finaliza las pruebas y devuelve el resultado
}

// Implementación de las pruebas de funciones para el controlador de CD
void test_Ctrl_CD(void)
{
    // Se establece el directorio actual
    char current_directory[1024];
    getcwd(current_directory, sizeof(current_directory));

    // Caso 1: No se proporciona argumento
    printf("Ejecutando prueba 1: Sin argumento...\n");
    strcpy(cwd, current_directory);                   // Establece el directorio actual
    Ctrl_CD(NULL);                                    // Debe imprimir el directorio actual
    TEST_ASSERT_EQUAL_STRING(cwd, current_directory); // Verifica que cwd no cambió
    // Obtener el directorio actual

    // Caso 2: Argumento válido (un directorio específico)
    printf("Ejecutando prueba 2: Argumento es un directorio...\n");
    Ctrl_CD("/home");                       // Debe cambiar al directorio especificado
    TEST_ASSERT_EQUAL_STRING(cwd, "/home"); // Verifica que cwd se actualizó correctamente

    // Caso 3: El argumento es "-"
    printf("Ejecutando prueba 3: Argumento es '-' ...\n");
    Ctrl_CD("-");                                     // Debe cambiar al último directorio
    TEST_ASSERT_EQUAL_STRING(cwd, current_directory); // Verifica que cwd se actualizó correctamente

    // Imprimir resultados
    printf("Todas las pruebas pasaron exitosamente.\n");
}

// Prueba de las funciones start_monitor y stop_monitor
void test_start_monitor_and_stop_monitor(void)
{

    // Caso 1: Se inicia monitor
    printf("Ejecutando prueba 1: Iniciar monitor...\n");
    start_monitor(); // Llamar a la función para iniciar el monitor

    // Verificar que el archivo contiene los datos esperados
    TEST_ASSERT_GREATER_THAN(0, monitor_pid); // Verificar que el monitor está en ejecución

    // Caso 2: Detener el monitor
    printf("Ejecutando prueba 2: Detener monitor...\n");
    stop_monitor(); // Detener el monitor

    // Verifica que el monitor se detuvo
    TEST_ASSERT_EQUAL_INT(MONITOR_OFF, monitor_pid);
}

// Prueba de la función ejecutar_comando_con_pipes
void test_ejecutar_comando_con_pipes(void)
{
    // Redirigir la salida estándar a un archivo temporal
    int fd = open("salida_test.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
    TEST_ASSERT_TRUE(fd >= 0); // Verificar que el archivo se abrió correctamente

    int stdout_backup = dup(STDOUT_FILENO); // Respaldar stdout
    dup2(fd, STDOUT_FILENO);                // Redirigir stdout al archivo
    close(fd);

    // Ejecutar un comando con pipes: echo "hola mundo" | grep "mundo"
    char comando[] = "echo hola mundo | grep mundo";
    ejecutar_comando_con_pipes(comando);

    // Restaurar stdout
    dup2(stdout_backup, STDOUT_FILENO);
    close(stdout_backup);

    // Leer el archivo de salida
    FILE* file = fopen("salida_test.txt", "r");
    TEST_ASSERT_NOT_NULL(file);

    char resultado[1024];
    fgets(resultado, sizeof(resultado), file);
    fclose(file);

    // Verificar la salida esperada
    TEST_ASSERT_EQUAL_STRING("hola mundo\n", resultado);

    // Eliminar archivo temporal
    remove("salida_test.txt");
}

void test_handle_sigterm(void)
{
    // Llamar a la función manejadora de SIGTERM
    handle_sigterm(SIGTERM);

    // Verificar que running ahora sea 0
    TEST_ASSERT_EQUAL_INT(0, running);
}
