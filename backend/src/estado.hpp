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
constexpr int    PERIODOS        = 6;        // bloques de clase por día (horario)
constexpr int    MAX_CURSOS_POR_BLOQUE = 3;  // cursos que pueden compartir un mismo bloque

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

// Un estudiante inscrito en un curso (módulo Inscripciones).
struct Inscripcion {
    std::string carnet;
    std::string curso;       // código del curso
};

// Aviso o fecha importante (módulo Avisos).
struct Aviso {
    int id = 0;
    std::string titulo;
    std::string descripcion;
    std::string fecha;       // "AAAA-MM-DD"
    std::string tipo;        // examen | entrega | evento | aviso
    std::string curso;       // código del curso relacionado, o "" si es general
};

struct Estado {
    std::vector<Estudiante>  estudiantes;
    std::vector<CursoEstado> cursos;
    std::vector<Inscripcion> inscripciones;
    std::vector<Aviso>       avisos;
    int siguiente_id_aviso = 1;

    // Horario: horario[día][periodo] = códigos de los cursos que se dan en ese bloque.
    std::vector<std::string> horario[DIAS_SEMANA][PERIODOS];

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
Resultado quitar_estudiante(Estado& e, const std::string& carnet);   // desplaza las filas de las matrices y quita sus inscripciones
Resultado agregar_curso(Estado& e, const CursoEstado& nuevo);
Resultado quitar_curso(Estado& e, const std::string& codigo);        // desplaza columnas y prerrequisitos; lo quita de inscripciones, horario y avisos

std::string normalizar_texto(const std::string& texto);              // minúsculas y sin tildes (para buscar y ordenar)
void ordenar_estudiantes(std::vector<Estudiante>& lista, const std::string& criterio);  // nombre | nombre-desc | carnet | semestre
