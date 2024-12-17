/**
 * @file signal_handlers.h
 * @brief Manejadores de señales para el shell.
 */
#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

/**
 * @brief Manejador de señales.
 * @param sig El número de señal recibido.
 *
 * Esta función maneja las señales comprobando si hay un proceso en primer plano.
 * Si hay un proceso en primer plano, envía la señal recibida a ese grupo de procesos.
 * De lo contrario, imprime una nueva línea, muestra el prompt y vacía el búfer de salida.
 */
void manejador_senales(int sig);
/**
 * @brief Manejador de la señal SIGCHLD.
 *
 * Esta función se llama cuando se recibe una señal SIGCHLD, indicando que un proceso hijo ha terminado.
 * Recoge el estado de todos los procesos hijos terminados y actualiza la lista de trabajos en consecuencia.
 *
 * La función realiza los siguientes pasos:
 * 1. Utiliza waitpid en un bucle con WNOHANG para recoger el estado de todos los procesos hijos terminados.
 * 2. Itera a través de la lista de trabajos para encontrar el trabajo correspondiente a cada proceso hijo terminado.
 * 3. Imprime un mensaje indicando la terminación del proceso hijo.
 * 4. Decrementa el contador de ID de trabajo y limpia el PID de la lista de trabajos.
 * 5. Llama a mostrar_prompt para mostrar el prompt después de manejar SIGCHLD.
 * 6. Vacía la salida estándar para asegurar que el prompt se muestre correctamente.
 */
void manejador_SIGCHLD(int sig __attribute__((unused)));

/**
 * @brief Manejador de señal para detener el programa.
 *
 * Esta función se llama cuando se recibe una señal SIGTERM.
 * Establece la variable global running en 0 para indicar que el programa debe detenerse.
 */
void handle_sigterm(int sig __attribute__((unused)));

#endif // SIGNAL_HANDLERS_H
