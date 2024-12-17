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

if(RUN_COVERAGE EQUAL 1)
    message(STATUS "Coverage enabled")
    enable_testing()
    add_subdirectory(tests)
    
else() 
    message(STATUS "Coverage disabled")

endif()

# Add the monitoring executable
add_subdirectory(Sistema-de-monitoreo-SO1)  # Agrega esta línea para incluir el directorio de monitoreo

# Add executable -> .exe/binary
add_executable(${PROJECT_NAME} src/main.c src/commands.c src/monitor.c src/shell_utils.c src/signal_handlers.c)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson unity::unity)  # Asegúrate de enlazar unity también
