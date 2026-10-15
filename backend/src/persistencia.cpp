// DUEÑO: P2 · Tareas: T24 — VERSIÓN BÁSICA
// Formato de los archivos en data/:
//   estudiantes.json -> [ {carnet, nombre, carrera, semestre}, ... ]
//   cursos.json      -> [ {codigo, nombre, creditos, semestre}, ... ]
//   notas.json       -> { "0901-26-1001": { "MAT101": 85, "PRG101": 90 }, ... }   (solo las notas que existen)
//   asistencia.json  -> { "0901-26-1001": [1,1,0,1,1], ... }
//   docentes.json    -> [ {id, nombre, especialidad}, ... ]
//   asignaciones.json-> [ {docenteId, cursoCodigo}, ... ]
//   asignaciones_estudiantes.json -> [ {carnet, curso}, ... ]   (inscripciones)
//   horario.json     -> { "celdas": [ [ ["MAT101"], ... 6 periodos ], ... 5 días ] }
//   avisos.json      -> [ {id, titulo, descripcion, fecha, tipo, curso}, ... ]
// cargar_todo() solo reemplaza los datos si LOS CUATRO primeros archivos existen y son válidos;
// si no, deja intactos los datos que ya están en memoria (los de ejemplo). Los otros son
// opcionales: si faltan o están dañados se usan los de ejemplo solo para esa parte.
// El árbol y el grafo son de MUESTRA: se arman con los cursos y los prerrequisitos del plan, no se guardan.
// main.cpp llama a guardar_todo() después de cada cambio exitoso (guardado automático).
// TODO(P2, T24): copias de seguridad en data/backup/.
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
#include "avisos.hpp"
#include "horario.hpp"
#include "inscripciones.hpp"
#include "docentes.hpp"
#include "respuestas.hpp"
#include "rutas_catalogo.hpp"

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

// Los ids de docentes son "D01", "D02"...; si el archivo trae números (1, 2...) se convierten.
std::string id_docente(const json& valor) {
    if (valor.is_string()) return valor.get<std::string>();
    if (valor.is_number_integer()) {
        const std::string n = std::to_string(valor.get<long long>());
        return "D" + (n.size() < 2 ? "0" + n : n);
    }
    throw std::runtime_error("id de docente inválido");
}

// Lee docentes.json y asignaciones.json. Devuelve false si docentes.json no existe o no sirve.
// `hay_asignaciones` queda en false si asignaciones.json falta o está vacío.
bool leer_docentes(const std::string& dir, std::vector<Docente>& docentes, std::vector<Asignacion>& asignaciones,
                   bool& hay_asignaciones) {
    json j_doc, j_asg;
    if (leer_archivo(dir + "/docentes.json", j_doc) != Lectura::OK || !j_doc.is_array()) return false;
    try {
        for (const json& d : j_doc)
            docentes.push_back({id_docente(d.at("id")), d.at("nombre").get<std::string>(), d.value("especialidad", std::string()), true});
        hay_asignaciones = leer_archivo(dir + "/asignaciones.json", j_asg) == Lectura::OK && j_asg.is_array();
        if (hay_asignaciones)
            for (const json& a : j_asg)
                asignaciones.push_back({id_docente(a.at("docenteId")), a.at("cursoCodigo").get<std::string>(), true});
    } catch (const std::exception& ex) {
        std::cerr << "[persistencia] docentes.json o asignaciones.json con formato inesperado: " << ex.what() << std::endl;
        return false;
    }
    return true;
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
    todo_bien = escribir_archivo(dir + "/asignaciones_estudiantes.json", inscripciones_a_json(e)) && todo_bien;
    todo_bien = escribir_archivo(dir + "/horario.json", horario_a_json(e)) && todo_bien;
    todo_bien = escribir_archivo(dir + "/avisos.json", avisos_a_json(e)) && todo_bien;


    // Docentes y asignaciones viven en el módulo de Cristian (con su propio candado).
    {
        std::lock_guard<std::mutex> candado(candado_docentes());
        json docentes = json::parse(obtenerDocentesJSON(), nullptr, false);
        json asignaciones = json::parse(obtenerAsignacionesJSON(), nullptr, false);
        if (docentes.is_array()) todo_bien = escribir_archivo(dir + "/docentes.json", docentes) && todo_bien;
        if (asignaciones.is_array()) todo_bien = escribir_archivo(dir + "/asignaciones.json", asignaciones) && todo_bien;
    }
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
    reconstruir_adyacencia(e);   // los prerrequisitos del plan se acomodan a los cursos cargados

    // Módulos extra: si su archivo falta o no sirve, se conservan los de ejemplo, pero filtrados
    // a los estudiantes y cursos que se acaban de cargar (para no apuntar a datos que no existen).
    json j_extra;
    if (leer_archivo(dir + "/asignaciones_estudiantes.json", j_extra) != Lectura::OK || !inscripciones_desde_json(j_extra, e))
        inscripciones_desde_json(inscripciones_a_json(e), e);
    if (leer_archivo(dir + "/horario.json", j_extra) != Lectura::OK || !horario_desde_json(j_extra, e))
        horario_desde_json(horario_a_json(e), e);
    if (leer_archivo(dir + "/avisos.json", j_extra) != Lectura::OK || !avisos_desde_json(j_extra, e))
        avisos_desde_json(avisos_a_json(e), e);

    std::vector<Docente> docentes;
    std::vector<Asignacion> asignaciones;
    bool hay_asignaciones = false;
    if (leer_docentes(dir, docentes, asignaciones, hay_asignaciones)) {
        std::lock_guard<std::mutex> candado(candado_docentes());
        if (!hay_asignaciones) {
            // Sin asignaciones.json se conservan las de ejemplo, solo las de docentes que siguen existiendo.
            json actuales = json::parse(obtenerAsignacionesJSON(), nullptr, false);
            if (actuales.is_array())
                for (const json& a : actuales)
                    for (const Docente& d : docentes)
                        if (a.value("docenteId", std::string()) == d.id)
                            asignaciones.push_back({d.id, a.value("cursoCodigo", std::string()), true});
        }
        if (!reemplazarDocentesYAsignaciones(docentes, asignaciones))
            std::cerr << "[persistencia] Demasiados docentes o asignaciones en data/; se dejan los de ejemplo." << std::endl;
    }
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
