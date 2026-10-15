#ifndef DOCENTES_HPP
#define DOCENTES_HPP

#include <string>
#include <vector>

struct Docente {
    std::string id;
    std::string nombre;
    std::string especialidad;
    bool activo;
};

struct Curso {
    std::string codigo;
    std::string nombre;
    int creditos;
    int semestre;
    bool activo;
};

struct Asignacion {
    std::string docenteId;
    std::string cursoCodigo;
    bool activo;
};

void inicializarDocentesYCursos();
std::string obtenerDocentesJSON();
std::string obtenerCursosJSON();
std::string obtenerAsignacionesJSON();

bool crearDocente(const std::string& nombre, const std::string& especialidad);
bool eliminarDocente(const std::string& id);

bool crearCurso(const std::string& codigo, const std::string& nombre, int creditos, int semestre);
bool eliminarCurso(const std::string& codigo);

bool crearAsignacion(const std::string& docenteId, const std::string& cursoCodigo);
bool eliminarAsignacion(const std::string& docenteId, const std::string& cursoCodigo);

// Persistencia: reemplaza docentes y asignaciones por los leídos de data/ (conserva los ids).
// Devuelve false si no caben (más de MAX_REGISTROS).
bool reemplazarDocentesYAsignaciones(const std::vector<Docente>& docentes, const std::vector<Asignacion>& asignaciones);

#endif