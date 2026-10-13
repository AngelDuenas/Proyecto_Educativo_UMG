// DUEÑO: P2 · Tareas: T24
// VERSIÓN BÁSICA: lee y escribe estudiantes, docentes, cursos, notas y asistencia.
// Formato de data/:
//   estudiantes.json -> [ {id, nombre, edad, grado, seccion}, ... ]
//   docentes.json    -> [ {id, nombre, especialidad}, ... ]
//   cursos.json      -> [ {id, nombre, docente_id}, ... ]
//   notas.json       -> [ [n, n, null, ...], ... ]   una fila por estudiante, una columna por curso
//   asistencia.json  -> [ [1, 0, 1, 1, null], ... ]  una fila por estudiante, una columna por día
// Si un archivo no existe o está vacío, simplemente se ignora (el sistema arranca con lo que haya).
// grafo.json y arbol.json los guardará/cargará P3.
// TODO(P2, T24): copias de seguridad en data/backup/, validaciones extra y manejo de archivos corruptos.
#include "persistencia.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "respuestas.hpp"

namespace {

enum class Lectura { NO_HAY, OK, INVALIDO };

// Lee un archivo JSON. NO_HAY = no existe o está vacío; INVALIDO = tiene contenido inválido.
Lectura leer_archivo(const std::string& ruta, json& salida) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo.is_open()) return Lectura::NO_HAY;
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    std::string texto = buffer.str();
    if (texto.find_first_not_of(" \t\r\n") == std::string::npos) return Lectura::NO_HAY;
    if (texto.compare(0, 3, "\xEF\xBB\xBF") == 0) texto.erase(0, 3);   // quita el BOM si lo hay
    salida = json::parse(texto, nullptr, false);
    return salida.is_discarded() ? Lectura::INVALIDO : Lectura::OK;
}

bool escribir_archivo(const std::string& ruta, const json& contenido) {
    std::ofstream archivo(ruta, std::ios::binary | std::ios::trunc);
    if (!archivo.is_open()) return false;
    archivo << contenido.dump(2, ' ', false, json::error_handler_t::replace) << "\n";
    return archivo.good();
}

}  // namespace

bool guardar_todo(Estado& e, const std::string& dir) {
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);

    json estudiantes = json::array();
    for (const Estudiante& s : e.estudiantes) {
        json j = json::object();
        j["id"] = s.id; j["nombre"] = s.nombre; j["edad"] = s.edad; j["grado"] = s.grado; j["seccion"] = s.seccion;
        estudiantes.push_back(j);
    }
    json docentes = json::array();
    for (const Docente& d : e.docentes) {
        json j = json::object();
        j["id"] = d.id; j["nombre"] = d.nombre; j["especialidad"] = d.especialidad;
        docentes.push_back(j);
    }
    json cursos = json::array();
    for (const Curso& c : e.cursos) {
        json j = json::object();
        j["id"] = c.id; j["nombre"] = c.nombre; j["docente_id"] = c.docente_id;
        cursos.push_back(j);
    }

    const size_t ne = std::min(e.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));
    const size_t nc = std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS));
    json notas = json::array();
    json asistencia = json::array();
    for (size_t i = 0; i < ne; ++i) {
        json fila_notas = json::array();
        for (size_t j = 0; j < nc; ++j) {
            if (e.notas[i][j] < 0.0f) fila_notas.push_back(nullptr);
            else                      fila_notas.push_back(redondear(static_cast<double>(e.notas[i][j])));
        }
        notas.push_back(fila_notas);

        json fila_asis = json::array();
        for (int d = 0; d < DIAS_SEMANA; ++d) {
            if (e.asistencia[i][d] == SIN_REGISTRO) fila_asis.push_back(nullptr);
            else                                    fila_asis.push_back(e.asistencia[i][d]);
        }
        asistencia.push_back(fila_asis);
    }

    bool todo_bien = true;
    todo_bien = escribir_archivo(dir + "/estudiantes.json", estudiantes) && todo_bien;
    todo_bien = escribir_archivo(dir + "/docentes.json", docentes) && todo_bien;
    todo_bien = escribir_archivo(dir + "/cursos.json", cursos) && todo_bien;
    todo_bien = escribir_archivo(dir + "/notas.json", notas) && todo_bien;
    todo_bien = escribir_archivo(dir + "/asistencia.json", asistencia) && todo_bien;
    return todo_bien;
}

bool cargar_todo(Estado& e, const std::string& dir) {
    json j_est, j_doc, j_cur, j_not, j_asi;
    const Lectura l_est = leer_archivo(dir + "/estudiantes.json", j_est);
    const Lectura l_doc = leer_archivo(dir + "/docentes.json", j_doc);
    const Lectura l_cur = leer_archivo(dir + "/cursos.json", j_cur);
    const Lectura l_not = leer_archivo(dir + "/notas.json", j_not);
    const Lectura l_asi = leer_archivo(dir + "/asistencia.json", j_asi);

    if (l_est == Lectura::INVALIDO || l_doc == Lectura::INVALIDO || l_cur == Lectura::INVALIDO ||
        l_not == Lectura::INVALIDO || l_asi == Lectura::INVALIDO) {
        std::cerr << "[persistencia] Hay un archivo JSON inválido en '" << dir << "'. No se cargó nada." << std::endl;
        return false;
    }

    // Primero se lee todo en variables temporales; el estado real solo se toca
    // si TODO se leyó bien.
    std::vector<Estudiante> estudiantes;
    std::vector<Docente> docentes;
    std::vector<Curso> cursos;
    std::vector<std::vector<float>> notas;
    std::vector<std::vector<int>> asistencia;

    try {
        if (l_est == Lectura::OK) {
            for (const json& s : j_est) {
                Estudiante x;
                x.id = s.at("id").get<int>();
                x.nombre = s.at("nombre").get<std::string>();
                x.edad = s.at("edad").get<int>();
                x.grado = s.at("grado").get<std::string>();
                x.seccion = s.at("seccion").get<std::string>();
                estudiantes.push_back(x);
            }
            if (estudiantes.size() > static_cast<size_t>(MAX_ESTUDIANTES)) throw std::runtime_error("demasiados estudiantes");
        }
        if (l_doc == Lectura::OK) {
            for (const json& d : j_doc) {
                Docente x;
                x.id = d.at("id").get<int>();
                x.nombre = d.at("nombre").get<std::string>();
                x.especialidad = d.at("especialidad").get<std::string>();
                docentes.push_back(x);
            }
        }
        if (l_cur == Lectura::OK) {
            for (const json& c : j_cur) {
                Curso x;
                x.id = c.at("id").get<int>();
                x.nombre = c.at("nombre").get<std::string>();
                x.docente_id = c.value("docente_id", -1);
                cursos.push_back(x);
            }
            if (cursos.size() > static_cast<size_t>(MAX_CURSOS)) throw std::runtime_error("demasiados cursos");
        }
        if (l_not == Lectura::OK) {
            for (const json& fila : j_not) {
                std::vector<float> f;
                for (const json& celda : fila) f.push_back(celda.is_null() ? SIN_NOTA : celda.get<float>());
                notas.push_back(f);
            }
        }
        if (l_asi == Lectura::OK) {
            for (const json& fila : j_asi) {
                std::vector<int> f;
                for (const json& celda : fila) f.push_back(celda.is_null() ? SIN_REGISTRO : celda.get<int>());
                asistencia.push_back(f);
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "[persistencia] Un archivo JSON no tiene el formato esperado: " << ex.what() << std::endl;
        return false;
    }

    // ---- Aplicar al estado ----
    if (l_est == Lectura::OK) {
        e.estudiantes = estudiantes;
        int mayor = 0;
        for (const Estudiante& x : e.estudiantes) mayor = std::max(mayor, x.id);
        e.siguiente_id_estudiante = mayor + 1;
    }
    if (l_doc == Lectura::OK) {
        e.docentes = docentes;
        int mayor = 0;
        for (const Docente& x : e.docentes) mayor = std::max(mayor, x.id);
        e.siguiente_id_docente = mayor + 1;
    }
    if (l_cur == Lectura::OK) {
        e.cursos = cursos;
        int mayor = 0;
        for (const Curso& x : e.cursos) mayor = std::max(mayor, x.id);
        e.siguiente_id_curso = mayor + 1;
    }

    // Si cambiaron los estudiantes o los cursos, las matrices viejas ya no sirven.
    const bool reiniciar_notas = (l_est == Lectura::OK || l_cur == Lectura::OK || l_not == Lectura::OK);
    const bool reiniciar_asistencia = (l_est == Lectura::OK || l_asi == Lectura::OK);
    if (reiniciar_notas) {
        for (int i = 0; i < MAX_ESTUDIANTES; ++i)
            for (int j = 0; j < MAX_CURSOS; ++j) e.notas[i][j] = SIN_NOTA;
    }
    if (reiniciar_asistencia) {
        for (int i = 0; i < MAX_ESTUDIANTES; ++i)
            for (int d = 0; d < DIAS_SEMANA; ++d) e.asistencia[i][d] = SIN_REGISTRO;
    }

    const size_t ne = std::min(e.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));
    const size_t nc = std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS));
    for (size_t i = 0; i < notas.size() && i < ne; ++i)
        for (size_t j = 0; j < notas[i].size() && j < nc; ++j) e.notas[i][j] = notas[i][j];
    for (size_t i = 0; i < asistencia.size() && i < ne; ++i)
        for (size_t d = 0; d < asistencia[i].size() && d < static_cast<size_t>(DIAS_SEMANA); ++d) e.asistencia[i][d] = asistencia[i][d];

    return true;
}

void registrar_rutas_persistencia(httplib::Server& svr, Estado& estado) {
    svr.Post("/api/guardar", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        if (!guardar_todo(estado)) {
            responder_error(res, "No se pudieron guardar los datos en la carpeta data/.", 500);
            return;
        }
        json datos = json::object();
        datos["guardado"] = true;
        responder_ok(res, datos);
    });

    svr.Post("/api/cargar", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        if (!cargar_todo(estado)) {
            responder_error(res, "No se pudieron cargar los datos: revisa los archivos de la carpeta data/.", 500);
            return;
        }
        json datos = json::object();
        datos["cargado"] = true;
        responder_ok(res, datos);
    });
}
