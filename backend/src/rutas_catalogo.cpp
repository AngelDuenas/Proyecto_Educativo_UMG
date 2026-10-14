// DUEÑO: P1 (conecta el código de Cristian con el servidor HTTP)
//   GET/POST/DELETE /api/docentes        -> docentes.cpp de Cristian
//   GET/POST/DELETE /api/asignaciones    -> docentes.cpp de Cristian
//   GET             /api/arbol           -> arbol.cpp de Cristian
//   GET/POST/DELETE /api/cursos          -> catálogo en Estado (así las notas y el grafo siempre
//                                           usan los mismos cursos que ve la pantalla de Docentes y Cursos)
#include "rutas_catalogo.hpp"

#include <mutex>
#include <string>

#include "arbol.hpp"
#include "docentes.hpp"
#include "respuestas.hpp"

namespace {

std::mutex mtx_docentes;   // el módulo de Cristian no es seguro entre hilos: se protege aquí

json ok_simple() {
    json ok = json::object();
    ok["ok"] = true;
    return ok;
}

json curso_a_json(const CursoEstado& c) {
    json j = json::object();
    j["codigo"] = c.codigo;
    j["nombre"] = c.nombre;
    j["creditos"] = c.creditos;
    j["semestre"] = c.semestre;
    return j;
}

bool leer_texto(const json& c, const char* campo, std::string& destino) {
    if (!c.contains(campo) || !c.at(campo).is_string()) return false;
    const std::string v = c.at(campo).get<std::string>();
    const size_t ini = v.find_first_not_of(" \t\r\n");
    if (ini == std::string::npos) return false;
    destino = v.substr(ini, v.find_last_not_of(" \t\r\n") - ini + 1);
    return true;
}

int leer_entero(const json& c, const char* campo, int por_defecto) {
    if (c.contains(campo) && c.at(campo).is_number_integer()) return static_cast<int>(c.at(campo).get<long long>());
    return por_defecto;
}

// Las funciones de Cristian devuelven el JSON como texto; aquí se vuelve a leer para consultarlo.
json leer_lista(const std::string& texto) {
    json lista = json::parse(texto, nullptr, false);
    return lista.is_array() ? lista : json::array();
}

bool existe_docente(const std::string& id) {
    for (const json& d : leer_lista(obtenerDocentesJSON()))
        if (d.contains("id") && d.at("id") == id) return true;
    return false;
}

bool existe_asignacion(const std::string& docente_id, const std::string& curso_codigo) {
    for (const json& a : leer_lista(obtenerAsignacionesJSON()))
        if (a.contains("docenteId") && a.at("docenteId") == docente_id && a.contains("cursoCodigo") && a.at("cursoCodigo") == curso_codigo)
            return true;
    return false;
}

// Quita todas las asignaciones que cumplan el filtro (por docente o por curso).
void quitar_asignaciones(const std::string& docente_id, const std::string& curso_codigo) {
    for (const json& a : leer_lista(obtenerAsignacionesJSON())) {
        if (!a.contains("docenteId") || !a.contains("cursoCodigo")) continue;
        const std::string d = a.at("docenteId").get<std::string>();
        const std::string c = a.at("cursoCodigo").get<std::string>();
        if ((!docente_id.empty() && d == docente_id) || (!curso_codigo.empty() && c == curso_codigo)) eliminarAsignacion(d, c);
    }
}

}  // namespace

void registrar_rutas_catalogo(httplib::Server& svr, Estado& estado) {
    // ---------- Docentes ----------
    svr.Get("/api/docentes", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(mtx_docentes);
        responder_json_texto(res, obtenerDocentesJSON());
    });

    svr.Post("/api/docentes", [](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        std::string nombre, especialidad;
        if (!leer_texto(cuerpo, "nombre", nombre)) {
            responder_error(res, "El nombre del docente es obligatorio.", 400);
            return;
        }
        leer_texto(cuerpo, "especialidad", especialidad);
        std::lock_guard<std::mutex> candado(mtx_docentes);
        if (!crearDocente(nombre, especialidad)) {
            responder_error(res, "No se pudo registrar al docente (se alcanzó el límite).", 400);
            return;
        }
        responder_json(res, ok_simple(), 201);
    });

    svr.Delete(R"(/api/docentes/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        const std::string id = req.matches[1].str();
        std::lock_guard<std::mutex> candado(mtx_docentes);
        if (!eliminarDocente(id)) {
            responder_error(res, "No existe el docente " + id + ".", 404);
            return;
        }
        quitar_asignaciones(id, "");   // al eliminar al docente se quitan sus asignaciones
        responder_json(res, ok_simple());
    });

    // ---------- Cursos (catálogo en Estado) ----------
    svr.Get("/api/cursos", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        json arreglo = json::array();
        for (const CursoEstado& c : estado.cursos) arreglo.push_back(curso_a_json(c));
        responder_json(res, arreglo);
    });

    svr.Post("/api/cursos", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        CursoEstado nuevo;
        if (!leer_texto(cuerpo, "codigo", nuevo.codigo) || !leer_texto(cuerpo, "nombre", nuevo.nombre)) {
            responder_error(res, "El código y el nombre son obligatorios.", 400);
            return;
        }
        nuevo.creditos = leer_entero(cuerpo, "creditos", 0);
        nuevo.semestre = leer_entero(cuerpo, "semestre", 1);

        std::lock_guard<std::mutex> candado(estado.mtx);
        const Resultado r = agregar_curso(estado, nuevo);   // agrega la columna a la matriz de notas
        if (r == Resultado::DUPLICADO) {
            responder_error(res, "Ya existe un curso con el código " + nuevo.codigo + ".", 409);
            return;
        }
        if (r == Resultado::LLENO) {
            responder_error(res, "Se alcanzó el máximo de cursos (" + std::to_string(MAX_CURSOS) + ").", 400);
            return;
        }
        responder_json(res, curso_a_json(nuevo), 201);
    });

    svr.Delete(R"(/api/cursos/([^/]+))", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string codigo = req.matches[1].str();
        {
            std::lock_guard<std::mutex> candado(estado.mtx);
            if (quitar_curso(estado, codigo) == Resultado::NO_EXISTE) {   // quita la columna y los prerrequisitos
                responder_error(res, "No existe el curso " + codigo + ".", 404);
                return;
            }
        }
        std::lock_guard<std::mutex> candado(mtx_docentes);
        quitar_asignaciones("", codigo);
        responder_json(res, ok_simple());
    });

    // ---------- Asignaciones ----------
    svr.Get("/api/asignaciones", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(mtx_docentes);
        responder_json_texto(res, obtenerAsignacionesJSON());
    });

    svr.Post("/api/asignaciones", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        std::string docente_id, curso_codigo;
        if (!leer_texto(cuerpo, "docenteId", docente_id) || !leer_texto(cuerpo, "cursoCodigo", curso_codigo)) {
            responder_error(res, "Faltan el docente y el curso.", 400);
            return;
        }
        {
            std::lock_guard<std::mutex> candado(estado.mtx);
            if (indice_curso(estado, curso_codigo) < 0) {
                responder_error(res, "No existe el curso " + curso_codigo + ".", 404);
                return;
            }
        }
        std::lock_guard<std::mutex> candado(mtx_docentes);
        if (!existe_docente(docente_id)) {
            responder_error(res, "No existe el docente " + docente_id + ".", 404);
            return;
        }
        if (existe_asignacion(docente_id, curso_codigo)) {
            responder_error(res, "Ese docente ya tiene asignado ese curso.", 409);
            return;
        }
        if (!crearAsignacion(docente_id, curso_codigo)) {
            responder_error(res, "No se pudo crear la asignación (se alcanzó el límite).", 400);
            return;
        }
        json datos = json::object();
        datos["docenteId"] = docente_id;
        datos["cursoCodigo"] = curso_codigo;
        responder_json(res, datos, 201);
    });

    svr.Delete(R"(/api/asignaciones/([^/]+)/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(mtx_docentes);
        if (!eliminarAsignacion(req.matches[1].str(), req.matches[2].str())) {
            responder_error(res, "No existe esa asignación.", 404);
            return;
        }
        responder_json(res, ok_simple());
    });

    // ---------- Árbol académico ----------
    svr.Get("/api/arbol", [](const httplib::Request&, httplib::Response& res) {
        responder_json_texto(res, obtenerArbolJSON());
    });
}
