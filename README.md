# Shell personalizada en C

Trabajo práctico de **Sistemas Operativos I** — FCEFyN, Universidad Nacional de Córdoba.

Una shell estilo Bourne escrita en C, que además controla el [monitor de métricas del sistema](https://github.com/Lenox30/Sistema-de-monitoreo-SO1) desarrollado en el práctico anterior (incluido como submódulo). La consigna original de la cátedra está en [CONSIGNA.md](CONSIGNA.md).

## Qué hace

- **Prompt** con usuario, host y directorio actual.
- **Comandos internos:** `cd` (actualiza `PWD` y `OLDPWD`, y soporta `cd -`), `clr`, `echo` con variables de entorno y `quit`.
- **Programas externos** con `fork()` y `execvp()`, con rutas relativas y absolutas.
- **Modo batch:** `./ShellProject comandos.txt` ejecuta los comandos de un archivo.
- **Segundo plano** con `&`, más control de trabajos con `fg` y `bg`.
- **Pipes** (`|`) y **redirección** de entrada y salida (`<`, `>`).
- **Señales:** `CTRL-C`, `CTRL-Z` y `CTRL-\` van al proceso en primer plano y no a la shell; `SIGCHLD` limpia los procesos hijos terminados.
- **Integración con el monitor:** `start_monitor`, `stop_monitor` y `status_monitor`; `update_config` y `explorar_config` leen y escriben su configuración en JSON (cJSON).

## Calidad

- Build con **CMake** y dependencias con **Conan**.
- **Tests** en `tests/` con reporte de cobertura.
- Pipeline de **GitHub Actions** que en cada pull request verifica el estilo (clang-format), la documentación (Doxygen), compila y corre los tests con cobertura.

## Cómo compilar y ejecutar

```bash
git clone --recurse-submodules https://github.com/Lenox30/Shell-Personalizada-SO1.git
cd Shell-Personalizada-SO1
mkdir build && cd build
cmake ..
make
./ShellProject              # modo interactivo
./ShellProject comandos.txt # modo batch
```

Los requisitos de instalación están en [INSTALL.md](INSTALL.md).

## Estructura

```
src/        main, comandos internos, utilidades, manejadores de señales, control del monitor
include/    headers
tests/      tests unitarios
```

## Qué aprendí

Creación y control de procesos (`fork`, `exec`, `wait`), comunicación entre procesos con pipes, manejo de señales del sistema operativo y organización de un proyecto en C con CMake, tests y CI.
