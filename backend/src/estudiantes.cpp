#include "estudiantes.hpp"
#include <sstream>

const int MAX_ESTUDIANTES = 100;
static Estudiante listaEstudiantes[MAX_ESTUDIANTES];
static int totalEstudiantes = 0;

void inicializarEstudiantes() {
    if (totalEstudiantes == 0) {
        listaEstudiantes[0] = {"0901-26-1001", "Ana Perez", "Ingenieria en Sistemas", 2, true};
        listaEstudiantes[1] = {"0901-26-1002", "Carlos Gomez", "Ingenieria Industrial", 1, true};
        totalEstudiantes = 2;
    }
}

std::string obtenerEstudiantesJSON(const std::string& buscar, const std::string& orden) {
    std::string resultado = "[";
    bool primero = true;

    for (int i = 0; i < totalEstudiantes; i++) {
        if (!listaEstudiantes[i].activo) continue;

        if (!primero) {
            resultado += ",";
        }
        primero = false;

        resultado += "{";
        resultado += "\"carnet\":\"" + listaEstudiantes[i].carnet + "\",";
        resultado += "\"nombre\":\"" + listaEstudiantes[i].nombre + "\",";
        resultado += "\"carrera\":\"" + listaEstudiantes[i].carrera + "\",";
        resultado += "\"semestre\":" + std::to_string(listaEstudiantes[i].semestre);
        resultado += "}";
    }

    resultado += "]";
    return resultado;
}

bool crearEstudiante(const std::string& carnet, const std::string& nombre, const std::string& carrera, int semestre) {
    if (totalEstudiantes >= MAX_ESTUDIANTES) return false;

    for (int i = 0; i < totalEstudiantes; i++) {
        if (listaEstudiantes[i].carnet == carnet && listaEstudiantes[i].activo) {
            return false; 
        }
    }

    listaEstudiantes[totalEstudiantes] = {carnet, nombre, carrera, semestre, true};
    totalEstudiantes++;
    return true;
}

bool actualizarEstudiante(const std::string& carnet, const std::string& nombre, const std::string& carrera, int semestre) {
    for (int i = 0; i < totalEstudiantes; i++) {
        if (listaEstudiantes[i].carnet == carnet && listaEstudiantes[i].activo) {
            listaEstudiantes[i].nombre = nombre;
            listaEstudiantes[i].carrera = carrera;
            listaEstudiantes[i].semestre = semestre;
            return true;
        }
    }
    return false;
}

bool eliminarEstudiante(const std::string& carnet) {
    for (int i = 0; i < totalEstudiantes; i++) {
        if (listaEstudiantes[i].carnet == carnet && listaEstudiantes[i].activo) {
            listaEstudiantes[i].activo = false;
            return true;
        }
    }
    return false;
}