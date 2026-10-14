// DUEÑO: P1 · estado compartido del sistema (estudiantes, cursos, notas, asistencia)
// Sin librerías externas: aquí vive la LÓGICA; las rutas HTTP están en otros archivos.
#pragma once
#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

constexpr int    MAX_ESTUDIANTES = 100;
constexpr int    MAX_CURSOS      = 15;
constexpr int    DIAS_SEMANA     = 5;        // L M X J V
constexpr float  SIN_NOTA        = -1.0f;    // valor centinela en la matriz de notas
constexpr double NOTA_APROBACION = 61.0;     // 61 o más aprueba (lo usa el frontend)

struct Estudiante {
    std::string carnet;      // identificador único, ej. "0901-26-1001"
    std::string nombre;
    std::string carrera;
    int semestre = 1;
};

// Curso del catálogo. Cada curso es una COLUMNA de la matriz de notas.
struct CursoEstado {
    std::string codigo;      // identificador único, ej. "MAT101"
    std::string nombre;
    int creditos = 0;
    int semestre = 1;
};

struct Estado {
    std::vector<Estudiante>  estudiantes;
    std::vector<CursoEstado> cursos;

    // Convención: FILA = posición del estudiante en `estudiantes`,
    //             COLUMNA = posición del curso en `cursos`.
    float notas[MAX_ESTUDIANTES][MAX_CURSOS];        // SIN_NOTA si no hay nota
    int   asistencia[MAX_ESTUDIANTES][DIAS_SEMANA];  // 1 presente, 0 ausente
    int   adyacencia[MAX_CURSOS][MAX_CURSOS];        // [i][j] = 1 si el curso i es prerrequisito del curso j

    std::mutex mtx;   // bloquear en cada handler que lea o modifique el estado
};

enum class Resultado { OK, DUPLICADO, LLENO, NO_EXISTE };

Estado& estado_global();                         // la única instancia del sistema
void inicializar_matrices(Estado& e);            // notas = SIN_NOTA, asistencia = 1, adyacencia = 0
void cargar_datos_ejemplo(Estado& e);            // 8 estudiantes, 10 cursos, notas, asistencia, prerrequisitos
void reconstruir_adyacencia(Estado& e);          // vuelve a armar la matriz de prerrequisitos de ejemplo

int indice_estudiante(const Estado& e, const std::string& carnet);   // -1 si no existe
int indice_curso(const Estado& e, const std::string& codigo);        // -1 si no existe

Resultado agregar_estudiante(Estado& e, const Estudiante& nuevo);
Resultado quitar_estudiante(Estado& e, const std::string& carnet);   // desplaza las filas de las matrices
Resultado agregar_curso(Estado& e, const CursoEstado& nuevo);
Resultado quitar_curso(Estado& e, const std::string& codigo);        // desplaza las columnas y la matriz de prerrequisitos

std::string normalizar_texto(const std::string& texto);              // minúsculas y sin tildes (para buscar y ordenar)
void ordenar_estudiantes(std::vector<Estudiante>& lista, const std::string& criterio);  // nombre | nombre-desc | carnet | semestre
