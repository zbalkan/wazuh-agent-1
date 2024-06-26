cmake_minimum_required(VERSION 3.10)

project(POC)

# Establecer el estándar de C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir los archivos fuente al proyecto
add_executable(POC 
    main.cpp
    command_receiver.cpp
    command_executor.cpp
)

# Incluir directorios
target_include_directories(POC PUBLIC "${PROJECT_SOURCE_DIR}")

# Enlazar bibliotecas
find_package(SQLite3 REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(POC PRIVATE SQLite::SQLite3 nlohmann_json::nlohmann_json)
