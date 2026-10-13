#ifndef ESTUDIANTES_HPP
#define ESTUDIANTES_HPP

#include <string>


struct Estudiante {
    std::string carnet;
    std::string nombre;
    std::string carrera;
    int semestre;
    bool activo; 
};

void inicializarEstudiantes();
std::string obtenerEstudiantesJSON(const std::string& buscar, const std::string& orden);
bool crearEstudiante(const std::string& carnet, const std::string& nombre, const std::string& carrera, int semestre);
bool actualizarEstudiante(const std::string& carnet, const std::string& nombre, const std::string& carrera, int semestre);
bool eliminarEstudiante(const std::string& carnet);

#endif