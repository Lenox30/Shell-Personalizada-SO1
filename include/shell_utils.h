/**
 * @file shell_utils.h
 * @brief Funciones auxiliares para la shell interactiva.
 */
#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

/**
 * @brief Inicializa la shell interactiva.
 *
 * Esta función verifica si la shell se está ejecutando de manera interactiva
 * y realiza las configuraciones necesarias para manejar señales y controlar
 * el terminal. Si la shell está en modo interactivo, se asegura de que esté
 * en primer plano, ignora ciertas señales de control de trabajos, maneja
 * señales específicas como SIGINT, SIGQUIT, SIGTSTP, SIGCHLD y SIGTERM, y
 * configura el grupo de procesos y los atributos del terminal.
 *
 * @note Esta función debe ser llamada al inicio del programa para configurar
 *       correctamente la shell interactiva.
 *
 * @see https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
void inicializar_shell(void);

/**
 * @brief Carga la configuración desde un archivo JSON y la guarda en un archivo.
 *
 * Esta función crea un objeto JSON que contiene un intervalo de muestreo y un array de métricas.
 * Luego convierte el objeto JSON a una cadena y la escribe en un archivo de configuración.
 *
 * @note La función utiliza la biblioteca cJSON para manejar los objetos JSON.
 *
 * @details
 * - Crea un objeto JSON raíz.
 * - Agrega un número al objeto JSON para el intervalo de muestreo.
 * - Crea un array JSON y agrega varias métricas al array.
 * - Agrega el array de métricas al objeto JSON raíz.
 * - Convierte el objeto JSON a una cadena.
 * - Abre un archivo de configuración para escritura.
 * - Escribe la cadena JSON en el archivo y cierra el archivo.
 * - Libera la memoria utilizada por la cadena JSON y el objeto JSON.
 *
 * @warning Si no se puede crear el objeto JSON, el array JSON, o la cadena JSON,
 *          o si no se puede abrir el archivo de configuración para escritura,
 *          se imprimirá un mensaje de error y la función retornará sin realizar ninguna acción.
 */
void load_config(void);

/**
 * @brief Muestra el prompt personalizado en la terminal.
 *
 * Esta función obtiene el nombre de usuario, el nombre del host y el directorio
 * actual para mostrar un prompt personalizado en la terminal. Si el directorio
 * actual es el directorio home del usuario, muestra "~" en lugar de la ruta completa.
 * Si el directorio actual es un subdirectorio del directorio home, muestra la ruta
 * relativa al home. En otros casos, muestra la ruta completa del directorio actual.
 *
 * Variables:
 * - usuario: Almacena el nombre de usuario.
 * - hostname: Almacena el nombre del host.
 * - prompt_dir: Almacena la ruta del prompt.
 * - home_dir: Almacena la ruta del home.
 *
 * Funciones utilizadas:
 * - getenv(): Obtiene el valor de una variable de entorno.
 * - gethostname(): Obtiene el nombre del host.
 * - getcwd(): Obtiene el directorio de trabajo actual.
 * - snprintf(): Escribe una cadena formateada en un buffer.
 * - strcmp(): Compara dos cadenas.
 * - strstr(): Encuentra la primera aparición de una subcadena en una cadena.
 * - printf(): Imprime una cadena formateada en la salida estándar.
 * - perror(): Imprime un mensaje de error en la salida estándar de error.
 */
void mostrar_prompt(void);

/** @brief Libera todos los recursos y termina todos los procesos y hilos.
 *
 * Esta función se encarga de liberar todos los recursos utilizados por el programa,
 * incluyendo la terminación de todos los procesos y hilos en ejecución, el cierre
 * de archivos abiertos, y la liberación de memoria dinámica.
 */
void liberar_recursos(void);

#endif // SHELL_UTILS_H
