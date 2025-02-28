/**
 * @file commands.c
 * @brief Implementación de las funciones para analizar y ejecutar comandos en el shell
 */

#include "commands.h"
#include "globals.h"
#include "monitor.h"
#include "shell_utils.h"
#include "signal_handlers.h"
#include <dirent.h>
#include <limits.h>

// Variables globales
/**
 *  @brief Directorio actual
 */
char cwd[PATH_MAX];

/**
 *  @brief Bandera para salir del shell
 */
int EXIT = 1;

// Variables para el manejo de trabajos en segundo plano
/**
 *  @brief ID del trabajo en segundo plano
 */
pid_t jobs[MAX_JOBS];

/**
 *  @brief Lista para almacenar los PIDs de los trabajos en segundo plano
 */

/**
 *  @brief PID del proceso en primer plano
 */
pid_t proceso_en_primer_plano = 0; // PID del proceso en primer plano

/**
 *  @brief ID del grupo de procesos de la shell
 */
int job_id = 1; // ID para los trabajos en segundo plano

// Analiza comandos y ejecuta acciones correspondientes
int analizar_comando(char* comando)
{
    char comando_copy[MAX_LINE];              // Copia del comando para evitar cambios
    strncpy(comando_copy, comando, MAX_LINE); // Copiar el comando a la variable de copia

    char* comando_base = strtok(comando_copy, " "); // Obtener el comando base
    char* argumento = strtok(NULL, " ");            // Obtener el argumento del comando

    // Verificar si el comando contiene un pipe
    if (strchr(comando, '|') != NULL)
    {
        ejecutar_comando_con_pipes(comando);
        return 0;
    }

    // Verificar si el comando es "cd"
    if (comando_base != NULL && strcmp(comando_base, "cd") == 0)
    {
        Ctrl_CD(argumento); // Llamar a la función para cambiar el directorio
        return 0;           // Indicar que el comando fue procesado
    }

    // Verificar si el comando es "clr"
    if (comando_base != NULL && strcmp(comando_base, "clr") == 0)
    {
        limpiar_pantalla(); // Llamar a la función para limpiar la pantalla
        return 0;           // Indicar que el comando fue procesado
    }

    // Verificar si el comando es "echo"
    if (comando_base != NULL && strcmp(comando_base, "echo") == 0 && argumento[strlen(argumento) - 1] != '&')
    {
        ejecutar_echo(argumento); // Llamar a la función para imprimir el texto
        return 0;
    }

    // Verificar si el comando es "quit"
    if (comando_base != NULL && strcmp(comando_base, "quit") == 0)
    {
        EXIT = 0;           // Cambiar el valor de la variable para salir del shell
        liberar_recursos(); // Llamar a la función para liberar los recursos
        return 0;           // Indicar que el comando fue procesado
    }

    // Verificar si el comando es "fg"
    if (comando_base != NULL && strcmp(comando_base, "fg") == 0)
    {
        int job_number = argumento ? atoi(argumento) : -1; // Obtener el número de trabajo
        manejar_comando_fg(job_number);
        return 0; // Indicar que el comando fue procesado
    }

    // Verificar si el comando es "bg"
    if (comando_base != NULL && strcmp(comando_base, "bg") == 0)
    {
        int job_number = argumento ? atoi(argumento) : -1; // Obtener el número de trabajo
        manejar_comando_bg(job_number);
        return 0; // Indicar que el comando fue procesado
    }

    // Verificar si el comando es "start_monitor"
    if (comando_base != NULL && strcmp(comando_base, "start_monitor") == 0)
    {
        start_monitor(); // Llamar a la función para iniciar el monitor
        return 0;        // Indicar que el comando fue procesado
    }

    // Verificar si el comando es "stop_monitor"
    if (comando_base != NULL && strcmp(comando_base, "stop_monitor") == 0)
    {
        stop_monitor(); // Llamar a la función para detener el monitor
        return 0;       // Indicar que el comando fue procesado
    }

    // Verifica si el comando es "status_monitor"
    if (comando_base != NULL && strcmp(comando_base, "status_monitor") == 0)
    {
        status_monitor(); // Llamar a la función para mostrar el estado del monitor
        return 0;         // Indicar que el comando fue procesado
    }

    // Verifica si el comando es "update_config"
    if (comando_base != NULL && strcmp(comando_base, "update_config") == 0)
    {
        handle_update_command(comando); // Llamar a la función para actualizar la configuración
        return 0;                       // Indicar que el comando fue procesado
    }

    // Verifica si el comando es "explorar_config"
    if (comando_base != NULL && strcmp(comando_base, "explorar_config") == 0)
    {
        const char* directorio = argumento ? argumento : cwd; // Si no se pasa directorio, usar el actual
        const char* extension = ".config";                    // Usar .config como ejemplo, pero puede ser .json u otro

        printf("Explorando el directorio: %s en busca de archivos '%s'\n", directorio, extension);
        buscar_configuraciones(directorio, extension); // Llamar a la función de búsqueda
        return 0;                                      // Indicar que el comando fue procesado
    }

    // Interpretar cualquier otro comando como un programa externo
    ejecutar_programa_externo(comando); // Llamar a la función para ejecutar un programa externo
    return 0;                           // Indicar que el comando fue procesado
}

// Controlador para el comando "cd"
void Ctrl_CD(char* argumento)
{
    // Si no se proporciona argumento, mostrar el directorio actual
    if (argumento == NULL || strlen(argumento) == 0)
    {
        printf("Directorio actual: %s\n", cwd);
        return;
    }

    // Si el comando es "cd -", cambiar al último directorio
    if (strcmp(argumento, "-") == 0)
    {
        cambiar_a_oldpwd();
        actualizar_pwd();
    }
    else
    {
        cambiar_a_directorio(argumento); // Cambiar al directorio especificado
        actualizar_pwd();                // Solo actualizar PWD si el cambio fue exitoso
    }
}

// Cambiar al directorio anterior
void cambiar_a_oldpwd()
{

    char* old_pwd = getenv("OLDPWD"); // Obtener el valor de OLDPWD
    if (old_pwd != NULL)
    {
        printf("%s\n", old_pwd);  // Mostrar el último directorio antes de cambiar
        if (chdir(old_pwd) == -1) // Cambiar al directorio anterior
        {
            perror("cd error");
        }
    }
    else
    {
        fprintf(stderr, "OLDPWD no está definido.\n"); // Mostrar un mensaje de error si OLDPWD no está definido
    }
}

// Cambiar al directorio especificado
void cambiar_a_directorio(const char* directorio)
{
    if (chdir(directorio) == -1) // Cambiar al directorio especificado
    {
        perror("cd error"); // Mostrar error si no se puede cambiar el directorio
    }
}

// Actualizar las variables de entorno PWD y OLDPWD
void actualizar_pwd()
{
    char old_cwd[PATH_MAX];
    strcpy(old_cwd, cwd); // Guardar el directorio actual

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        setenv("OLDPWD", old_cwd, 1); // Establecer OLDPWD al directorio anterior
        setenv("PWD", cwd, 1);        // Actualizar PWD al nuevo directorio
    }
    else
    {
        perror("getcwd() error");
    }
}

// Limpiar la pantalla
void limpiar_pantalla()
{
    if (system("clear")) // Limpiar la pantalla usando el comando "clear"
    {
        perror("clear error"); // Mostrar un mensaje de error si falla
    }
}

// Imprimir el texto en la consola
void ejecutar_echo(char* argumento)
{
    // Guardar los descriptores de archivo originales
    int stdout_fd = dup(STDOUT_FILENO); // Descriptor de archivo para stdout
    int stdin_fd = dup(STDIN_FILENO);   // Descriptor de archivo para stdin
    char* args[MAX_LINE];               // Variable para almacenar el texto a imprimir
    int i = 0;                          // Contador de argumentos
    bool redireccion = false;           // Bandera para redirección de salida

    if (argumento == NULL || strlen(argumento) == 0)
    {
        printf("\n");
        return;
    }

    // Tokeniza el comando y guarda cada argumento en args
    while (argumento != NULL)
    {
        if (strcmp(argumento, ">") == 0 || strcmp(argumento, "<") == 0) // Verificar si se requiere redirección
        {
            redireccion = true;
        }
        args[i++] = argumento;         // Guardar el argumento
        argumento = strtok(NULL, " "); // Tokeniza el argumento
    }
    args[i] = NULL; // Agregar NULL al final de la lista de argumentos

    // Verificar si se requiere redirección
    if (redireccion)
    {
        manejar_redirecciones(args);
    }

    for (int j = 0; args[j] != NULL; j++) // Recorrer todos los argumentos
    {
        if (args[j][0] == '$') // Verificar si el argumento es una variable de entorno
        {
            char* var_nombre = args[j] + 1;   // Saltar el '$'
            char* valor = getenv(var_nombre); // Obtener el valor de la variable
            if (valor != NULL)
            {
                printf("%s", valor); // Imprimir el valor de la variable
            }
            else
            {
                printf(" "); // Si la variable no existe, imprimir un espacio
            }
        }
        else
        {

            printf("%s", args[j]); // Imprimir el argumento
        }

        printf(" "); // Imprimir un espacio entre los argumentos
    }
    printf("\n"); // Imprimir una nueva línea al final

    if (redireccion)
    {
        // Restaurar los descriptores de archivo originales
        dup2(stdout_fd, STDOUT_FILENO);
        dup2(stdin_fd, STDIN_FILENO);
        close(stdout_fd);
        close(stdin_fd);
    }

    return;
}

// Ejecutar un programa externo
void ejecutar_programa_externo(char* comando)
{

    char* args[MAX_LINE];          // Lista de argumentos
    int i = 0;                     // Contador de argumentos
    bool en_segundo_plano = false; // Verifica si se ejecuta en segundo plano

    if (comando[strlen(comando) - 1] == '&') // Verifica si el comando debe ejecutarse en segundo plano
    {
        en_segundo_plano = true;
        comando[strlen(comando) - 1] = '\0'; // Elimina el '&' del comando
    }

    // Tokeniza el comando y guarda cada argumento en args
    char* token = strtok(comando, " ");
    while (token != NULL)
    {
        // Eliminar comillas simples si están presentes
        if (token[0] == '\'' && token[strlen(token) - 1] == '\'')
        {
            token[strlen(token) - 1] = '\0';
            token++;
        }
        args[i++] = token;         // Guardar el argumento
        token = strtok(NULL, " "); // Tokeniza el argumento
    }
    args[i] = NULL; // Agregar NULL al final de la lista de argumentos

    if (args[0] == NULL)
        return; // Si no hay comando, salir

    pid_t pid = fork(); // Crear un proceso hijo
    if (pid < 0)
    {
        perror("Error al crear el proceso hijo");
    }
    else if (pid == 0) // Código del proceso hijo
    {

        setpgid(0, 0);               // Establecer el grupo de procesos del hijo
        manejar_redirecciones(args); // Llama a la función de redirecciones

        if (execvp(args[0], args) == -1) // ejectuar programa
        {
            perror("Error al ejecutar el programa");
            exit(EXIT_FAILURE);
        }
    }
    else // Código del proceso padre
    {
        setpgid(pid, pid); // Establecer el grupo de procesos del hijo
        if (en_segundo_plano)
        {
            int j;
            for (j = 0; j < MAX_JOBS && jobs[j] != 0; j++)
                ;             // Encuentra espacio en jobs
            if (j < MAX_JOBS) // Si hay espacio, agrega el trabajo
            {
                jobs[j] = pid;                      // Guardar el PID en la lista de trabajos
                printf("[%d] %d\n", job_id++, pid); // Imprimir el trabajo en segundo plano
            }
            else
            {
                printf("Máximo de trabajos en segundo plano alcanzado.\n");
            }
        }
        else // Si no se ejecuta en segundo plano
        {
            proceso_en_primer_plano = pid;               // Establecer el proceso en primer plano
            int status;                                  // Variable para almacenar el estado del proceso
            while (waitpid(pid, &status, WUNTRACED) > 0) // Espera a que el proceso termine
            {
                if (WIFSTOPPED(status)) // Verifica si el proceso fue suspendido
                {
                    printf("\nProceso %d suspendido\n", pid);
                    proceso_en_primer_plano = 0; // Establecer el proceso en primer plano a 0
                    return;
                }
            }
            proceso_en_primer_plano = 0;
        }
    }
}

// Manejar el comando "fg"
void manejar_comando_fg(int job_number)
{
    pid_t pid = job_number; // Convertir el número de trabajo en un PID
    if (pid == -1)          // Verificar si el PID es válido
    {
        printf("Trabajo no encontrado\n");
        return;
    }

    // Enviar SIGCONT para reanudar el trabajo en primer plano
    if (kill(pid, SIGCONT) == 0)
    {
        // Esperar a que el proceso termine en primer plano
        proceso_en_primer_plano = pid;            // Establecer el proceso en primer plano
        int status;                               // Variable para almacenar el estado del proceso
        if (waitpid(pid, &status, WUNTRACED) > 0) // Esperar a que el proceso termine o sea suspendido
        {
            if (WIFSTOPPED(status))
            {
                printf("\nProceso %d suspendido\n", pid);
            }
            else
            {
                printf("Proceso %d terminado\n", pid);
            }
        }
        proceso_en_primer_plano = 0;
    }
    else
    {
        perror("Error al enviar SIGCONT");
    }
}

// Manejar el comando "bg"
void manejar_comando_bg(int job_number)
{
    pid_t pid = job_number; // Convertir el número de trabajo en un PID
    if (pid == -1)          // Verificar si el PID es válido
    {
        printf("Trabajo no encontrado\n");
        return;
    }

    // Enviar SIGCONT para reanudar el trabajo en segundo plano
    if (kill(pid, SIGCONT) == 0)
    {
        printf("Proceso %d reanudado en segundo plano\n", pid);
    }
    else
    {
        perror("Error al enviar SIGCONT");
    }

    // Agregar el proceso a la lista de trabajos en segundo plano
    int j; // Variable para el índice de trabajos
    for (j = 0; j < MAX_JOBS && jobs[j] != 0; j++)
        ; // Buscar espacio libre en la lista de trabajos

    if (j < MAX_JOBS)
    {
        jobs[j] = pid; // Guardar el PID en la lista de trabajos
    }
    else
    {
        printf("Máximo de trabajos en segundo plano alcanzado.\n");
    }
}

// Ejecutar un comando con pipes
void ejecutar_comando_con_pipes(char* comando)
{
    char* comandos[MAX_LINE]; // Array para almacenar los comandos divididos por |
    int num_comandos = 0;     // Número de comandos en la cadena

    // Dividir el comando en segmentos separados por |
    char* segmento = strtok(comando, "|"); // Tokenizar el comando por |
    while (segmento != NULL && num_comandos < MAX_LINE)
    {
        comandos[num_comandos++] = segmento;
        segmento = strtok(NULL, "|");
    }

    int pipefd[2];               // Pipe para conectar procesos
    int input_fd = STDIN_FILENO; // Inicialmente, entrada estándar

    for (int i = 0; i < num_comandos; i++) // Iterar sobre todos los comandos
    {
        if (pipe(pipefd) == -1) // Verificar si hay un error al crear el pipe
        {
            perror("Error al crear el pipe");
            exit(EXIT_FAILURE);
        } // Crear un nuevo pipe

        pid_t pid = fork(); // Crear un nuevo proceso hijo
        if (pid == 0)       // Código del proceso hijo
        {
            // Redireccionar input_fd a entrada estándar
            dup2(input_fd, STDIN_FILENO);
            // Si no es el último comando, redirecciona salida al pipe
            if (i < num_comandos - 1)
            {
                dup2(pipefd[1], STDOUT_FILENO); // Redireccionar salida del pipe al nuevo pipe
            }
            close(pipefd[0]);                       // Cerrar el lado de lectura del pipe
            ejecutar_programa_externo(comandos[i]); // Ejecutar el comando actual
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            perror("Error en fork");
            return;
        }

        // En el padre, actualizar input_fd para la próxima iteración
        close(pipefd[1]);     // Cerrar lado de escritura del pipe
        input_fd = pipefd[0]; // Leer del pipe para el próximo comando
    }

    // Esperar a todos los procesos hijos
    while (wait(NULL) > 0)
        ;
}

// Manejar redirecciones de entrada y salida
void manejar_redirecciones(char** args)
{
    int in = -1, out = -1; // Descriptores para los archivos de entrada y salida
    int i = 0;             // Índice para recorrer los argumentos

    // Recorre los argumentos en busca de operadores de redirección
    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">") == 0)
        { // Redirección de salida
            if (args[i + 1] != NULL)
            { // Verifica si hay un archivo de salida
                out = open(args[i + 1], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR); // Abre el archivo de salida
                if (out == -1)
                {
                    perror("Error al abrir archivo de salida");
                    return;
                }
                dup2(out, STDOUT_FILENO); // Redirige stdout al archivo
                close(out);               // Cierra el descriptor de archivo
                args[i] = NULL;           // Remueve el operador y archivo de args
            }
            else
            {
                fprintf(stderr, "Error: falta el archivo para redirección de salida\n");
                return;
            }
        }
        else if (strcmp(args[i], "<") == 0)
        { // Redirección de entrada
            if (args[i + 1] != NULL)
            {                                     // Verifica si hay un archivo de entrada
                in = open(args[i + 1], O_RDONLY); // Abre el archivo de entrada
                if (in == -1)
                {
                    perror("Error al abrir archivo de entrada");
                    return;
                }
                dup2(in, STDIN_FILENO); // Redirige stdin al archivo
                close(in);              // Cierra el descriptor de archivo
                args[i] = NULL;         // Remueve el operador y archivo de args
            }
            else
            {
                fprintf(stderr, "Error: falta el archivo para redirección de entrada\n");
                return;
            }
        }
        i++; // Incrementa el índice
    }
}

void buscar_configuraciones(const char* directorio, const char* extension)
{
    DIR* dir = opendir(directorio);
    if (dir == NULL)
    {
        perror("No se puede abrir el directorio");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Ignorar los directorios "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char ruta_completa[512];
        snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", directorio, entry->d_name);

        // Si es un directorio, hacer llamada recursiva
        if (entry->d_type == DT_DIR)
        {
            buscar_configuraciones(ruta_completa, extension);
        }
        else
        {
            // Comprobar si el archivo tiene la extensión especificada
            if (strstr(entry->d_name, extension) != NULL)
            {
                printf("Archivo de configuración encontrado: %s\n", ruta_completa);
                FILE* archivo = fopen(ruta_completa, "r");
                if (archivo)
                {
                    char linea[256];
                    printf("Contenido de %s:\n", ruta_completa);
                    while (fgets(linea, sizeof(linea), archivo))
                    {
                        printf("%s", linea);
                    }
                    fclose(archivo);
                }
                else
                {
                    perror("Error al abrir el archivo");
                }
            }
        }
    }
    closedir(dir);
}