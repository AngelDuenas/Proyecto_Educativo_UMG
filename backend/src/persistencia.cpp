// DUEÑO: P2 · Tareas: T24 — VERSIÓN BÁSICA
// Formato de los archivos en data/:
//   estudiantes.json -> [ {carnet, nombre, carrera, semestre}, ... ]
//   cursos.json      -> [ {codigo, nombre, creditos, semestre}, ... ]
//   notas.json       -> { "0901-26-1001": { "MAT101": 85, "PRG101": 90 }, ... }   (solo las notas que existen)
//   asistencia.json  -> { "0901-26-1001": [1,1,0,1,1], ... }
// cargar_todo() solo reemplaza los datos si LOS CUATRO archivos existen y son válidos;
// si no, deja intactos los datos que ya están en memoria (los de ejemplo).
// TODO(P2, T24): copias de seguridad en data/backup/ y guardado de los prerrequisitos (grafo.json).
#include "persistencia.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "algoritmos.hpp"
#include "respuestas.hpp"

namespace {

enum class Lectura { NO_HAY, OK, INVALIDO };

Lectura leer_archivo(const std::string& ruta, json& salida) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo.is_open()) return Lectura::NO_HAY;
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    std::string texto = buffer.str();
    if (texto.find_first_not_of(" \t\r\n") == std::string::npos) return Lectura::NO_HAY;   // vacío
    if (texto.compare(0, 3, "\xEF\xBB\xBF") == 0) texto.erase(0, 3);                       // quita el BOM
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

    const size_t ne = std::min(e.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));
    const size_t nc = std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS));

    json estudiantes = json::array(), cursos = json::array();
    json notas = json::object(), asistencia = json::object();

    for (size_t i = 0; i < ne; ++i) {
        const Estudiante& s = e.estudiantes[i];
        json j = json::object();
        j["carnet"] = s.carnet; j["nombre"] = s.nombre; j["carrera"] = s.carrera; j["semestre"] = s.semestre;
        estudiantes.push_back(j);

        json notas_estudiante = json::object();
        for (size_t c = 0; c < nc; ++c)
            if (e.notas[i][c] >= 0.0f) notas_estudiante[e.cursos[c].codigo] = redondear(static_cast<double>(e.notas[i][c]));
        notas[s.carnet] = notas_estudiante;

        json fila = json::array();
        for (int d = 0; d < DIAS_SEMANA; ++d) fila.push_back(e.asistencia[i][d]);
        asistencia[s.carnet] = fila;
    }
    for (size_t c = 0; c < nc; ++c) {
        json j = json::object();
        j["codigo"] = e.cursos[c].codigo; j["nombre"] = e.cursos[c].nombre;
        j["creditos"] = e.cursos[c].creditos; j["semestre"] = e.cursos[c].semestre;
        cursos.push_back(j);
    }

    bool todo_bien = true;
    todo_bien = escribir_archivo(dir + "/estudiantes.json", estudiantes) && todo_bien;
    todo_bien = escribir_archivo(dir + "/cursos.json", cursos) && todo_bien;
    todo_bien = escribir_archivo(dir + "/notas.json", notas) && todo_bien;
    todo_bien = escribir_archivo(dir + "/asistencia.json", asistencia) && todo_bien;
    return todo_bien;
}

bool cargar_todo(Estado& e, const std::string& dir) {
    json j_est, j_cur, j_not, j_asi;
    const Lectura l[4] = {
        leer_archivo(dir + "/estudiantes.json", j_est), leer_archivo(dir + "/cursos.json", j_cur),
        leer_archivo(dir + "/notas.json", j_not),       leer_archivo(dir + "/asistencia.json", j_asi)};
    for (const Lectura& x : l) {
        if (x != Lectura::OK) {
            std::cerr << "[persistencia] No se cargaron los archivos de '" << dir << "' (faltan, están vacíos o son inválidos). Se usan los datos de ejemplo." << std::endl;
            return false;
        }
    }

    // Primero se lee todo en variables temporales; el estado real solo se toca si TODO se leyó bien.
    std::vector<Estudiante> estudiantes;
    std::vector<CursoEstado> cursos;
    std::map<std::string, std::map<std::string, double>> notas;
    std::map<std::string, std::vector<int>> asistencia;
    try {
        if (!j_est.is_array() || !j_cur.is_array() || !j_not.is_object() || !j_asi.is_object())
            throw std::runtime_error("estructura inesperada");
        for (const json& s : j_est) {
            Estudiante x;
            x.carnet = s.at("carnet").get<std::string>();
            x.nombre = s.at("nombre").get<std::string>();
            x.carrera = s.value("carrera", std::string());
            x.semestre = s.value("semestre", 1);
            estudiantes.push_back(x);
        }
        for (const json& c : j_cur) {
            CursoEstado x;
            x.codigo = c.at("codigo").get<std::string>();
            x.nombre = c.at("nombre").get<std::string>();
            x.creditos = c.value("creditos", 0);
            x.semestre = c.value("semestre", 1);
            cursos.push_back(x);
        }
        if (estudiantes.size() > static_cast<size_t>(MAX_ESTUDIANTES)) throw std::runtime_error("demasiados estudiantes");
        if (cursos.size() > static_cast<size_t>(MAX_CURSOS)) throw std::runtime_error("demasiados cursos");

        for (auto it = j_not.begin(); it != j_not.end(); ++it)
            for (auto n = it.value().begin(); n != it.value().end(); ++n)
                if (n.value().is_number()) notas[it.key()][n.key()] = n.value().get<double>();
        for (auto it = j_asi.begin(); it != j_asi.end(); ++it) {
            std::vector<int> fila;
            for (const json& v : it.value()) fila.push_back(v.is_number() && v.get<double>() != 0.0 ? 1 : 0);
            asistencia[it.key()] = fila;
        }
    } catch (const std::exception& ex) {
        std::cerr << "[persistencia] Formato inesperado en data/: " << ex.what() << ". Se usan los datos de ejemplo." << std::endl;
        return false;
    }

    // ---- Aplicar al estado ----
    e.estudiantes = estudiantes;
    e.cursos = cursos;
    for (int i = 0; i < MAX_ESTUDIANTES; ++i) {
        for (int c = 0; c < MAX_CURSOS; ++c)  e.notas[i][c] = SIN_NOTA;
        for (int d = 0; d < DIAS_SEMANA; ++d) e.asistencia[i][d] = 1;
    }
    for (size_t i = 0; i < e.estudiantes.size(); ++i) {
        const auto fn = notas.find(e.estudiantes[i].carnet);
        if (fn != notas.end()) {
            for (size_t c = 0; c < e.cursos.size(); ++c) {
                const auto nota = fn->second.find(e.cursos[c].codigo);
                if (nota != fn->second.end()) e.notas[i][c] = static_cast<float>(nota->second);
            }
        }
        const auto fa = asistencia.find(e.estudiantes[i].carnet);
        if (fa != asistencia.end())
            for (size_t d = 0; d < fa->second.size() && d < static_cast<size_t>(DIAS_SEMANA); ++d) e.asistencia[i][d] = fa->second[d];
    }
    reconstruir_adyacencia(e);   // los prerrequisitos de ejemplo se acomodan a los cursos cargados
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
        responder_json(res, datos);
    });

    svr.Post("/api/cargar", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        if (!cargar_todo(estado)) {
            responder_error(res, "No se pudieron cargar los datos: revisa los archivos de la carpeta data/.", 500);
            return;
        }
        json datos = json::object();
        datos["cargado"] = true;
        responder_json(res, datos);
    });
}
