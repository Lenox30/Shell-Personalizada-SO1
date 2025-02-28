# Verify cmake version
cmake_minimum_required(VERSION 3.28)

# Create a project
project(ShellProject VERSION 1.0.0 DESCRIPTION "A nice shell project" LANGUAGES C)

# Enable Testing
include(CTest)

# Flags for compiling 
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -Wall -Wpedantic -Werror -Wextra -Wconversion -Wunused-parameter -Wmissing-prototypes -Wstrict-prototypes ")

# Includes headers
include_directories(include)

# Find dependencies
find_package(cJSON REQUIRED) 
find_package(unity REQUIRED)

# Agregar el sistema de monitoreo
add_subdirectory(Sistema-de-monitoreo-SO1)

# **Crear ejecutable principal**
add_executable(ShellProject 
    src/main.c 
    src/commands.c 
    src/monitor.c 
    src/shell_utils.c 
    src/signal_handlers.c
)

# Enlazar librerías
target_link_libraries(ShellProject PRIVATE cjson::cjson unity::unity)

# Forzar que el binario se almacene en `bin/`
set_target_properties(ShellProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Agregar el directorio de tests si está activada la cobertura
if(RUN_COVERAGE EQUAL 1)
    message(STATUS "Coverage enabled")
    enable_testing()
    add_subdirectory(tests)
else() 
    message(STATUS "Coverage disabled")
endif()
