# Intrucciones para para compilar, ejecutar y utilizar la shell y el programa de monitoreo.

## Paso 1: Clonar el repositorio
Para clonar de manera adecuada el repositorio con sus respectivo submodulo debemo ejecutar el comando:
   ```bash
  git clone https://github.com/ICOMP-UNC/so-i-24-chp2-Lenox30.git

   ```
Luego cambiar a la rama:
   ```bash
git switch Lenox2.0
   ```
Por ultimo debemos clonar el sub modulo ya que al trabajar con una rama distinta a master no utilizar: "git clone --recurse-submodules"
   ```bash
cd Sistema-de-monitoreo-SO1/
git submodule update --init --recursive
cd ..
   ```

## Paso 2: Crear un entorno virtual
Un entorno virtual permite instalar y gestionar dependencias específicas de un proyecto sin afectar al sistema global de Python.
Para ello debemos crear una carpeta llamada venv:
   ```bash
mkdir venv
   ```
Luego:
   ```bash
python3 -m venv venv
   ```
Por ultimo activamos el entorno virtual con:
   ```bash
source venv/bin/activate
   ```
## Paso 3: Instalar Conan e instalar dependencias con el mismo:
Conan es un gestor de paquetes para C y C++, que facilita la gestión de dependencias y la configuración de proyectos en múltiples plataformas.
Para instalarlo debemos utiliar el comando:
   ```bash
pip install conan
   ```
Una vez instalado, debemos crear una carpeta llamada build, donde se alacenaran las dependencias luego:
   ```bash
mkdir build
   ```
Ahora podemos instalar las dependencias usando:
   ```bash
conan install . --build=missing --profile=debug
   ```
## Paso 4: Generación de archivos de construcción con CMAKE
CMake se utiliza para generar archivos de construcción (como Makefiles o proyectos de Visual Studio) a partir de scripts de configuración, facilitando la compilación y gestión de proyectos.
Para ello debemos ejecutar:
   ```bash
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_COVERAGE=1
   ```
En este comando se habilitan los tests con la bandera "-DRUN_COVERAGE=1", si usted no quiere ver los test podria eliminar esta bandera.

## Paso 5: Compilar proyecto:
Para compilar el proyecto simplemente debemos ejecutar el comando:
   ```bash
make 
   ```
## Paso 6: Ejecutar el proyecto
Para ejecutar el proyecto y utilizarlo hay que ejecutar el programa creado luego de la compilacion con:
   ```bash
./ShellProject
   ```
# Ejecutar Tests
Si usted decidio no quitar la bandera de test podra ejecutar e incluse verlos en formato httml siguiendo estos pasos:

## Paso 1: Ejecutar test
Para ejecutar los test, obligatoriamente se debe hacer los pasos para ""compilar, ejecutar y utilizar la shell y el programa de monitoreo" escritos anteriormente
Luego ejecutar el programa de test con:
   ```bash
./tests/test_shell
   ```
## Paso 2:Con LCOV capturar información de cobertura de código
LCOV captura los datos de cobertura generados en el directorio actual y los guarda en un archivo llamado coverage.info. Luego, este archivo puede ser procesado y analizado para generar informes detallados de cobertura de código, o para visualizarla en herramientas como genhtml para obtener una representación en HTML de la cobertura de pruebas.
Para ello debes ejecutar el comando:
   ```bash
lcov --capture --directory . --output-file coverage.info
   ```
## Paso 3: Generar generar un informe visual de la cobertura de código
Utiliza:
   ```bash
genhtml coverage.info --output-directory coverage
   ```
## Paso 4: Visualizar el informe por medio de tu navegador web
Por ultimo para visualizar el informe en tu navegador web utiliza el siguiente y ultimo comando:
Para firefox:
   ```bash
firefox coverage/index.html
   ```
Para google crhome:
   ```bash
google-chrome coverage/index.html 
   ```
