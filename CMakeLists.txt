cmake_minimum_required(VERSION 3.10)
project(gestion_escolar CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB FUENTES ${CMAKE_SOURCE_DIR}/backend/src/*.cpp)
add_executable(gestion_escolar ${FUENTES})

target_include_directories(gestion_escolar PRIVATE ${CMAKE_SOURCE_DIR}/backend/src)
target_include_directories(gestion_escolar SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/backend/third_party)
target_compile_options(gestion_escolar PRIVATE -Wall -Wextra -O2)

find_package(Threads REQUIRED)
target_link_libraries(gestion_escolar PRIVATE Threads::Threads)

if(WIN32)
    target_compile_definitions(gestion_escolar PRIVATE _WIN32_WINNT=0x0A00)
    target_link_libraries(gestion_escolar PRIVATE ws2_32 wsock32)
endif()

set_target_properties(gestion_escolar PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
