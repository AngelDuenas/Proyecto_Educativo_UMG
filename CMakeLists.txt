# DUEÑO: P5 · Tareas: T02 (repo y guía de compilación)
# Alternativa a make:
#   cmake -S . -B build && cmake --build build
#   ./build/gestion_escolar   (ejecutar desde la raíz del repo)
cmake_minimum_required(VERSION 3.14)
project(gestion_escolar CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# El ejecutable queda directamente en build/ (también con generadores multi-config).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_BINARY_DIR})
endforeach()

file(GLOB FUENTES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/backend/src/*.cpp)
add_executable(gestion_escolar ${FUENTES})

find_package(Threads REQUIRED)
target_link_libraries(gestion_escolar PRIVATE Threads::Threads)

if(MSVC)
    target_compile_options(gestion_escolar PRIVATE /W4 /utf-8 /bigobj /EHsc)
    target_compile_definitions(gestion_escolar PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(gestion_escolar PRIVATE -O2 -Wall -Wextra)
endif()

if(WIN32)
    target_compile_definitions(gestion_escolar PRIVATE _WIN32_WINNT=0x0A00)
    target_link_libraries(gestion_escolar PRIVATE ws2_32 wsock32 crypt32)
endif()
