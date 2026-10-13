// DUEÑO: P5 + P1 · Tareas: T08
// Estructuras base y estado global del sistema.
// IMPORTANTE: no cambiar los nombres ni las dimensiones sin avisar al equipo
// (y anotarlo en docs/DECISIONES.md).
#pragma once
#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

constexpr int   MAX_ESTUDIANTES  = 100;
constexpr int   MAX_CURSOS       = 10;
constexpr int   DIAS_SEMANA      = 5;      // L M X J V
constexpr float SIN_NOTA         = -1.0f;  // valor centinela en la matriz de notas
constexpr float NOTA_APROBACION  = 60.0f;
constexpr int   SIN_REGISTRO     = -1;     // asistencia: 1 presente, 0 ausente, -1 sin registro

struct Estudiante { int id; std::string nombre; int edad; std::string grado; std::string seccion; };
struct Docente    { int id; std::string nombre; std::string especialidad; };
struct Curso      { int id; std::string nombre; int docente_id; };   // docente_id = -1 si no tiene

struct Nodo {                       // árbol de la estructura académica
    std::string nombre;
    std::vector<Nodo*> hijos;
};

struct Estado {
    std::vector<Estudiante> estudiantes;
    std::vector<Docente>    docentes;
    std::vector<Curso>      cursos;

    // Matrices. La FILA es la posición del estudiante en el vector `estudiantes`;
    // la COLUMNA es la posición del curso en el vector `cursos`.
    float notas[MAX_ESTUDIANTES][MAX_CURSOS];
    int   asistencia[MAX_ESTUDIANTES][DIAS_SEMANA];
    int   adyacencia[MAX_CURSOS][MAX_CURSOS];   // adyacencia[i][j] = 1 si el curso i es prerrequisito del curso j

    Nodo* raiz_arbol = nullptr;

    int siguiente_id_estudiante = 1;
    int siguiente_id_docente    = 1;
    int siguiente_id_curso      = 1;

    std::mutex mtx;                 // bloquear en cada handler que lea o modifique el estado
};

void inicializar_matrices(Estado& e);                       // notas = SIN_NOTA, asistencia = SIN_REGISTRO, adyacencia = 0
void quitar_estudiante_en(Estado& e, size_t indice);        // elimina al estudiante Y desplaza las filas de notas y asistencia
int  indice_estudiante_por_id(const Estado& e, int id);     // -1 si no existe
int  indice_curso_por_id(const Estado& e, int id);          // -1 si no existe
