#include "docentes.hpp"

#include <algorithm>

const int MAX_REGISTROS = 50;

static Docente listaDocentes[MAX_REGISTROS];
static int totalDocentes = 0;

static Curso listaCursos[MAX_REGISTROS];
static int totalCursos = 0;

static Asignacion listaAsignaciones[MAX_REGISTROS];
static int totalAsignaciones = 0;

void inicializarDocentesYCursos() {
    if (totalDocentes == 0) {
        listaDocentes[0] = {"D01", "Ing. Carlos Mendez", "Matematica", true};
        listaDocentes[1] = {"D02", "Inga. Lucia Herrera", "Programacion", true};
        totalDocentes = 2;
    }
    if (totalCursos == 0) {
        listaCursos[0] = {"MAT101", "Matematica I", 5, 1, true};
        listaCursos[1] = {"PRG101", "Programacion I", 5, 1, true};
        totalCursos = 2;
    }
    if (totalAsignaciones == 0) {
        listaAsignaciones[0] = {"D01", "MAT101", true};
        listaAsignaciones[1] = {"D02", "PRG101", true};
        totalAsignaciones = 2;
    }
}

std::string obtenerDocentesJSON() {
    std::string r = "[";
    bool primero = true;
    for (int i = 0; i < totalDocentes; i++) {
        if (!listaDocentes[i].activo) continue;
        if (!primero) r += ",";
        primero = false;
        r += "{\"id\":\"" + listaDocentes[i].id + "\",";
        r += "\"nombre\":\"" + listaDocentes[i].nombre + "\",";
        r += "\"especialidad\":\"" + listaDocentes[i].especialidad + "\"}";
    }
    return r + "]";
}

std::string obtenerCursosJSON() {
    std::string r = "[";
    bool primero = true;
    for (int i = 0; i < totalCursos; i++) {
        if (!listaCursos[i].activo) continue;
        if (!primero) r += ",";
        primero = false;
        r += "{\"codigo\":\"" + listaCursos[i].codigo + "\",";
        r += "\"nombre\":\"" + listaCursos[i].nombre + "\",";
        r += "\"creditos\":" + std::to_string(listaCursos[i].creditos) + ",";
        r += "\"semestre\":" + std::to_string(listaCursos[i].semestre) + "}";
    }
    return r + "]";
}

std::string obtenerAsignacionesJSON() {
    std::string r = "[";
    bool primero = true;
    for (int i = 0; i < totalAsignaciones; i++) {
        if (!listaAsignaciones[i].activo) continue;
        if (!primero) r += ",";
        primero = false;
        r += "{\"docenteId\":\"" + listaAsignaciones[i].docenteId + "\",";
        r += "\"cursoCodigo\":\"" + listaAsignaciones[i].cursoCodigo + "\"}";
    }
    return r + "]";
}

static std::string siguienteIdDocente() {
    int mayor = 0;
    for (int i = 0; i < totalDocentes; i++) {
        const std::string& id = listaDocentes[i].id;
        if (id.size() > 1 && id[0] == 'D') {
            try {
                mayor = std::max(mayor, std::stoi(id.substr(1)));
            } catch (...) {
            }
        }
    }
    const std::string numero = std::to_string(mayor + 1);
    return "D" + (numero.size() < 2 ? "0" + numero : numero);
}

bool crearDocente(const std::string& nombre, const std::string& especialidad) {
    if (totalDocentes >= MAX_REGISTROS) return false;
    std::string nuevoId = siguienteIdDocente();
    listaDocentes[totalDocentes] = {nuevoId, nombre, especialidad, true};
    totalDocentes++;
    return true;
}

bool eliminarDocente(const std::string& id) {
    for (int i = 0; i < totalDocentes; i++) {
        if (listaDocentes[i].id == id && listaDocentes[i].activo) {
            listaDocentes[i].activo = false;
            return true;
        }
    }
    return false;
}

bool crearCurso(const std::string& codigo, const std::string& nombre, int creditos, int semestre) {
    if (totalCursos >= MAX_REGISTROS) return false;
    for (int i = 0; i < totalCursos; i++) {
        if (listaCursos[i].codigo == codigo && listaCursos[i].activo) return false;
    }
    listaCursos[totalCursos] = {codigo, nombre, creditos, semestre, true};
    totalCursos++;
    return true;
}

bool eliminarCurso(const std::string& codigo) {
    for (int i = 0; i < totalCursos; i++) {
        if (listaCursos[i].codigo == codigo && listaCursos[i].activo) {
            listaCursos[i].activo = false;
            return true;
        }
    }
    return false;
}

bool crearAsignacion(const std::string& docenteId, const std::string& cursoCodigo) {
    if (totalAsignaciones >= MAX_REGISTROS) return false;
    listaAsignaciones[totalAsignaciones] = {docenteId, cursoCodigo, true};
    totalAsignaciones++;
    return true;
}

bool eliminarAsignacion(const std::string& docenteId, const std::string& cursoCodigo) {
    for (int i = 0; i < totalAsignaciones; i++) {
        if (listaAsignaciones[i].docenteId == docenteId && listaAsignaciones[i].cursoCodigo == cursoCodigo && listaAsignaciones[i].activo) {
            listaAsignaciones[i].activo = false;
            return true;
        }
    }
    return false;
}

bool reemplazarDocentesYAsignaciones(const std::vector<Docente>& docentes, const std::vector<Asignacion>& asignaciones) {
    if (docentes.size() > static_cast<size_t>(MAX_REGISTROS) || asignaciones.size() > static_cast<size_t>(MAX_REGISTROS)) return false;
    totalDocentes = 0;
    for (const Docente& d : docentes) listaDocentes[totalDocentes++] = {d.id, d.nombre, d.especialidad, true};
    totalAsignaciones = 0;
    for (const Asignacion& a : asignaciones) listaAsignaciones[totalAsignaciones++] = {a.docenteId, a.cursoCodigo, true};
    return true;
}
