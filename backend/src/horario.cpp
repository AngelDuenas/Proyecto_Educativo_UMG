// Módulo Horarios. Formato definido por vista-horario.js.
#include "horario.hpp"

#include <algorithm>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "docentes.hpp"
#include "respuestas.hpp"
#include "rutas_catalogo.hpp"

namespace {

const char* NOMBRES_DIA[DIAS_SEMANA] = {"Lunes", "Martes", "Miércoles", "Jueves", "Viernes"};
const char* INICIO_PERIODO[PERIODOS] = {"07:00", "07:50", "08:40", "09:50", "10:40", "11:30"};
const char* FIN_PERIODO[PERIODOS]    = {"07:50", "08:40", "09:30", "10:40", "11:30", "12:20"};

// Horario de ejemplo: un texto por bloque con los códigos separados por espacio.
// El viernes 09:50 tiene MAT101 y PRG101 juntos a propósito: Ana Lucía está inscrita en los dos (choque).
const char* HORARIO_EJEMPLO[DIAS_SEMANA][PERIODOS] = {
    {"MAT101", "PRG101", "LOG101", "MAT102", "PRG102", "FIS101"},
    {"ALG201", "MDS201", "PRG201", "EDD301", "MAT101", "PRG101"},
    {"MAT102", "PRG102", "ALG201", "MDS201", "PRG201", "EDD301"},
    {"MAT101", "LOG101", "FIS101", "MAT102", "PRG102", "ALG201"},
    {"MDS201", "PRG201", "EDD301", "MAT101 PRG101", "LOG101", "FIS101"},
};

std::vector<std::string> separar(const std::string& texto) {
    std::vector<std::string> partes;
    std::string actual;
    for (char ch : texto + " ") {
        if (ch == ' ') {
            if (!actual.empty()) partes.push_back(actual);
            actual.clear();
        } else {
            actual += ch;
        }
    }
    return partes;
}

// Relaciones que se necesitan para detectar choques, tomadas de los otros módulos.
struct Contexto {
    std::map<std::string, std::vector<std::string>> docentes_de_curso;   // curso -> nombres de docentes
    std::map<std::string, std::set<std::string>> cursos_de_estudiante;   // carnet -> cursos inscritos
    std::map<std::string, std::string> nombre_estudiante;
};

Contexto armar_contexto(const Estado& e) {
    Contexto ctx;
    for (const Estudiante& s : e.estudiantes) ctx.nombre_estudiante[s.carnet] = s.nombre;
    for (const Inscripcion& x : e.inscripciones) ctx.cursos_de_estudiante[x.carnet].insert(x.curso);

    // Docentes y asignaciones viven en el módulo de Cristian (orden de candados: estado y luego docentes).
    std::lock_guard<std::mutex> candado(candado_docentes());
    std::map<std::string, std::string> nombre_docente;
    json docentes = json::parse(obtenerDocentesJSON(), nullptr, false);
    if (docentes.is_array())
        for (const json& d : docentes) nombre_docente[d.value("id", "")] = d.value("nombre", "");
    json asignaciones = json::parse(obtenerAsignacionesJSON(), nullptr, false);
    if (asignaciones.is_array())
        for (const json& a : asignaciones) {
            const std::string id = a.value("docenteId", "");
            ctx.docentes_de_curso[a.value("cursoCodigo", "")].push_back(nombre_docente.count(id) ? nombre_docente[id] : id);
        }
    return ctx;
}

// Choques de un bloque: docentes o estudiantes que aparecen en dos o más de sus cursos.
void buscar_choques(const std::vector<std::string>& bloque, int dia, int periodo, const Contexto& ctx, json& choques) {
    if (bloque.size() < 2) return;
    std::map<std::string, std::vector<std::string>> por_docente, por_estudiante;
    for (const std::string& curso : bloque) {
        const auto d = ctx.docentes_de_curso.find(curso);
        if (d != ctx.docentes_de_curso.end())
            for (const std::string& nombre : d->second) por_docente[nombre].push_back(curso);
        for (const auto& par : ctx.cursos_de_estudiante)
            if (par.second.count(curso)) por_estudiante[par.first].push_back(curso);
    }
    auto agregar = [&](const char* tipo, const std::string& quien, const std::vector<std::string>& cursos) {
        json c = json::object();
        c["dia"] = dia;
        c["periodo"] = periodo;
        c["tipo"] = tipo;
        c["quien"] = quien;
        c["cursos"] = cursos;
        choques.push_back(c);
    };
    for (const auto& par : por_docente)
        if (par.second.size() > 1) agregar("docente", par.first, par.second);
    for (const auto& par : por_estudiante)
        if (par.second.size() > 1) {
            const auto n = ctx.nombre_estudiante.find(par.first);
            agregar("estudiante", n != ctx.nombre_estudiante.end() ? n->second : par.first, par.second);
        }
}

bool leer_entero(const json& c, const char* campo, int& destino) {
    if (!c.contains(campo) || !c.at(campo).is_number_integer()) return false;
    destino = static_cast<int>(c.at(campo).get<long long>());
    return true;
}

}  // namespace

void cargar_ejemplo_horario(Estado& e) {
    for (int d = 0; d < DIAS_SEMANA; ++d)
        for (int p = 0; p < PERIODOS; ++p) e.horario[d][p] = separar(HORARIO_EJEMPLO[d][p]);
}

json horario_a_json(const Estado& e) {
    json celdas = json::array();
    for (int d = 0; d < DIAS_SEMANA; ++d) {
        json fila = json::array();
        for (int p = 0; p < PERIODOS; ++p) fila.push_back(e.horario[d][p]);
        celdas.push_back(fila);
    }
    json datos = json::object();
    datos["celdas"] = celdas;
    return datos;
}

bool horario_desde_json(const json& j, Estado& e) {
    if (!j.is_object() || !j.contains("celdas") || !j.at("celdas").is_array() || j.at("celdas").size() != DIAS_SEMANA) return false;
    std::vector<std::string> nuevo[DIAS_SEMANA][PERIODOS];
    for (int d = 0; d < DIAS_SEMANA; ++d) {
        const json& fila = j.at("celdas").at(static_cast<size_t>(d));
        if (!fila.is_array() || fila.size() != PERIODOS) return false;
        for (int p = 0; p < PERIODOS; ++p) {
            const json& bloque = fila.at(static_cast<size_t>(p));
            if (!bloque.is_array()) return false;
            for (const json& codigo : bloque)
                if (codigo.is_string() && indice_curso(e, codigo.get<std::string>()) >= 0 &&
                    nuevo[d][p].size() < static_cast<size_t>(MAX_CURSOS_POR_BLOQUE))
                    nuevo[d][p].push_back(codigo.get<std::string>());
        }
    }
    for (int d = 0; d < DIAS_SEMANA; ++d)
        for (int p = 0; p < PERIODOS; ++p) e.horario[d][p] = nuevo[d][p];
    return true;
}

void registrar_rutas_horario(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/horario", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string carnet = req.get_param_value("carnet");
        std::lock_guard<std::mutex> candado(estado.mtx);
        if (!carnet.empty() && indice_estudiante(estado, carnet) < 0) {
            responder_error(res, "No existe el estudiante " + carnet + ".", 404);
            return;
        }
        const Contexto ctx = armar_contexto(estado);
        const auto inscritos = ctx.cursos_de_estudiante.find(carnet);

        json dias = json::array(), periodos = json::array(), celdas = json::array(), choques = json::array();
        for (const char* d : NOMBRES_DIA) dias.push_back(d);
        for (int p = 0; p < PERIODOS; ++p) {
            json x = json::object();
            x["inicio"] = INICIO_PERIODO[p];
            x["fin"] = FIN_PERIODO[p];
            periodos.push_back(x);
        }

        std::map<std::string, int> bloques;
        for (int d = 0; d < DIAS_SEMANA; ++d) {
            json fila = json::array();
            for (int p = 0; p < PERIODOS; ++p) {
                std::vector<std::string> visibles;
                for (const std::string& codigo : estado.horario[d][p]) {
                    if (indice_curso(estado, codigo) < 0) continue;
                    if (!carnet.empty() && (inscritos == ctx.cursos_de_estudiante.end() || !inscritos->second.count(codigo))) continue;
                    visibles.push_back(codigo);
                    ++bloques[codigo];
                }
                json bloque = json::array();
                for (const std::string& codigo : visibles) {
                    const CursoEstado& c = estado.cursos[static_cast<size_t>(indice_curso(estado, codigo))];
                    json x = json::object();
                    x["codigo"] = c.codigo;
                    x["nombre"] = c.nombre;
                    x["semestre"] = c.semestre;
                    const auto doc = ctx.docentes_de_curso.find(codigo);
                    x["docentes"] = doc != ctx.docentes_de_curso.end() ? json(doc->second) : json::array();
                    bloque.push_back(x);
                }
                fila.push_back(bloque);
                buscar_choques(visibles, d, p, ctx, choques);
            }
            celdas.push_back(fila);
        }

        // Cuántos bloques tiene cada curso a la semana (para compararlo con sus créditos).
        json por_curso = json::array();
        for (const CursoEstado& c : estado.cursos) {
            if (!carnet.empty() && (inscritos == ctx.cursos_de_estudiante.end() || !inscritos->second.count(c.codigo))) continue;
            json x = json::object();
            x["codigo"] = c.codigo;
            x["nombre"] = c.nombre;
            x["creditos"] = c.creditos;
            x["bloques"] = bloques.count(c.codigo) ? bloques[c.codigo] : 0;
            por_curso.push_back(x);
        }

        json datos = json::object();
        datos["dias"] = dias;
        datos["periodos"] = periodos;
        datos["celdas"] = celdas;
        datos["choques"] = choques;
        datos["bloquesPorCurso"] = por_curso;
        responder_json(res, datos);
    });

    svr.Put("/api/horario", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        int dia = -1, periodo = -1;
        if (!leer_entero(cuerpo, "dia", dia) || dia < 0 || dia >= DIAS_SEMANA) {
            responder_error(res, "El día debe estar entre 0 (lunes) y 4 (viernes).", 400);
            return;
        }
        if (!leer_entero(cuerpo, "periodo", periodo) || periodo < 0 || periodo >= PERIODOS) {
            responder_error(res, "El periodo debe estar entre 0 y " + std::to_string(PERIODOS - 1) + ".", 400);
            return;
        }
        if (!cuerpo.contains("cursos") || !cuerpo.at("cursos").is_array()) {
            responder_error(res, "Falta la lista de cursos del bloque.", 400);
            return;
        }
        if (cuerpo.at("cursos").size() > static_cast<size_t>(MAX_CURSOS_POR_BLOQUE)) {
            responder_error(res, "Un bloque admite como máximo " + std::to_string(MAX_CURSOS_POR_BLOQUE) + " cursos.", 400);
            return;
        }

        std::lock_guard<std::mutex> candado(estado.mtx);
        std::vector<std::string> bloque;
        for (const json& x : cuerpo.at("cursos")) {
            if (!x.is_string() || indice_curso(estado, x.get<std::string>()) < 0) {
                responder_error(res, "No existe el curso " + (x.is_string() ? x.get<std::string>() : x.dump()) + ".", 404);
                return;
            }
            if (std::find(bloque.begin(), bloque.end(), x.get<std::string>()) != bloque.end()) {
                responder_error(res, "El curso " + x.get<std::string>() + " está repetido en el bloque.", 400);
                return;
            }
            bloque.push_back(x.get<std::string>());
        }
        estado.horario[dia][periodo] = bloque;

        // Se guarda igual, pero se avisa si el bloque quedó con algún choque.
        json choques = json::array();
        buscar_choques(bloque, dia, periodo, armar_contexto(estado), choques);
        json datos = json::object();
        datos["dia"] = dia;
        datos["periodo"] = periodo;
        datos["cursos"] = bloque;
        datos["choques"] = choques;
        responder_json(res, datos);
    });
}
