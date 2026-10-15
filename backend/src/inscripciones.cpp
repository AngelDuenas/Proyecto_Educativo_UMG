// Módulo Inscripciones. Formato definido por vista-inscripciones.js.
// Une tres estructuras: el vector de inscripciones, la matriz de notas (¿ya aprobó?)
// y la matriz de adyacencia del grafo (¿qué prerrequisitos tiene el curso?).
#include "inscripciones.hpp"

#include <algorithm>
#include <mutex>

#include "respuestas.hpp"

namespace {

bool aprobado(const Estado& e, size_t fila, size_t curso) {
    return e.notas[fila][curso] >= static_cast<float>(NOTA_APROBACION);
}

bool esta_inscrito(const Estado& e, const std::string& carnet, const std::string& curso) {
    for (const Inscripcion& x : e.inscripciones)
        if (x.carnet == carnet && x.curso == curso) return true;
    return false;
}

bool leer_texto(const json& c, const char* campo, std::string& destino) {
    if (!c.contains(campo) || !c.at(campo).is_string() || c.at(campo).get<std::string>().empty()) return false;
    destino = c.at(campo).get<std::string>();
    return true;
}

json nota_o_null(float nota) {
    if (nota < 0.0f) return json(nullptr);
    return json(static_cast<double>(nota));
}

}  // namespace

std::vector<int> prerrequisitos_pendientes(const Estado& e, size_t fila, size_t curso) {
    // Columna `curso` de la matriz de adyacencia: adyacencia[i][curso] = 1 si i es prerrequisito.
    std::vector<int> faltan;
    const size_t n = std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS));
    for (size_t i = 0; i < n; ++i)
        if (e.adyacencia[i][curso] == 1 && !aprobado(e, fila, i)) faltan.push_back(static_cast<int>(i));
    return faltan;
}

void cargar_ejemplo_inscripciones(Estado& e) {
    // Algunos estudiantes repiten cursos que reprobaron (nota < 61 en los datos de ejemplo).
    e.inscripciones = {
        {"0901-26-1001", "MAT101"}, {"0901-26-1001", "PRG101"},
        {"0901-26-1002", "PRG201"}, {"0901-26-1003", "MAT102"},
    };
}

json inscripciones_a_json(const Estado& e) {
    json lista = json::array();
    for (const Inscripcion& x : e.inscripciones) {
        json j = json::object();
        j["carnet"] = x.carnet;
        j["curso"] = x.curso;
        lista.push_back(j);
    }
    return lista;
}

bool inscripciones_desde_json(const json& j, Estado& e) {
    if (!j.is_array()) return false;
    std::vector<Inscripcion> leidas;
    for (const json& x : j) {
        Inscripcion i;
        if (!leer_texto(x, "carnet", i.carnet) || !leer_texto(x, "curso", i.curso)) return false;
        if (indice_estudiante(e, i.carnet) >= 0 && indice_curso(e, i.curso) >= 0) leidas.push_back(i);
    }
    e.inscripciones = leidas;
    return true;
}

void registrar_rutas_inscripciones(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/inscripciones", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        json lista = json::array();
        for (const Inscripcion& x : estado.inscripciones) {
            const int f = indice_estudiante(estado, x.carnet);
            const int c = indice_curso(estado, x.curso);
            if (f < 0 || c < 0) continue;
            json j = json::object();
            j["carnet"] = x.carnet;
            j["nombre"] = estado.estudiantes[static_cast<size_t>(f)].nombre;
            j["curso"] = x.curso;
            j["cursoNombre"] = estado.cursos[static_cast<size_t>(c)].nombre;
            lista.push_back(j);
        }
        responder_json(res, lista);
    });

    svr.Get(R"(/api/inscripciones/([^/]+))", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string carnet = req.matches[1].str();
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int f = indice_estudiante(estado, carnet);
        if (f < 0) {
            responder_error(res, "No existe el estudiante " + carnet + ".", 404);
            return;
        }
        const size_t fila = static_cast<size_t>(f);
        const Estudiante& s = estado.estudiantes[fila];

        json cursos = json::array();
        int creditos_inscritos = 0;
        for (size_t c = 0; c < estado.cursos.size() && c < static_cast<size_t>(MAX_CURSOS); ++c) {
            const CursoEstado& cur = estado.cursos[c];
            json faltan = json::array();
            std::string situacion;
            if (aprobado(estado, fila, c)) {
                situacion = "aprobado";
            } else if (esta_inscrito(estado, carnet, cur.codigo)) {
                situacion = "inscrito";
                creditos_inscritos += cur.creditos;
            } else {
                for (int p : prerrequisitos_pendientes(estado, fila, c)) {
                    json x = json::object();
                    x["codigo"] = estado.cursos[static_cast<size_t>(p)].codigo;
                    x["nombre"] = estado.cursos[static_cast<size_t>(p)].nombre;
                    faltan.push_back(x);
                }
                situacion = faltan.empty() ? "disponible" : "bloqueado";
            }
            json j = json::object();
            j["codigo"] = cur.codigo;
            j["nombre"] = cur.nombre;
            j["semestre"] = cur.semestre;
            j["creditos"] = cur.creditos;
            j["nota"] = nota_o_null(estado.notas[fila][c]);
            j["estado"] = situacion;
            j["faltan"] = faltan;
            cursos.push_back(j);
        }

        json estudiante = json::object();
        estudiante["carnet"] = s.carnet;
        estudiante["nombre"] = s.nombre;
        estudiante["semestre"] = s.semestre;
        json datos = json::object();
        datos["estudiante"] = estudiante;
        datos["cursos"] = cursos;
        datos["creditosInscritos"] = creditos_inscritos;
        responder_json(res, datos);
    });

    svr.Post("/api/inscripciones", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        std::string carnet, curso;
        if (!leer_texto(cuerpo, "carnet", carnet) || !leer_texto(cuerpo, "curso", curso)) {
            responder_error(res, "Faltan el carnet y el código del curso.", 400);
            return;
        }
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int f = indice_estudiante(estado, carnet);
        const int c = indice_curso(estado, curso);
        if (f < 0) {
            responder_error(res, "No existe el estudiante " + carnet + ".", 404);
            return;
        }
        if (c < 0) {
            responder_error(res, "No existe el curso " + curso + ".", 404);
            return;
        }
        if (esta_inscrito(estado, carnet, curso)) {
            responder_error(res, "El estudiante ya está inscrito en " + curso + ".", 409);
            return;
        }
        if (aprobado(estado, static_cast<size_t>(f), static_cast<size_t>(c))) {
            responder_error(res, "El estudiante ya aprobó " + curso + ".", 400);
            return;
        }
        const std::vector<int> faltan = prerrequisitos_pendientes(estado, static_cast<size_t>(f), static_cast<size_t>(c));
        if (!faltan.empty()) {
            std::string lista;
            for (size_t k = 0; k < faltan.size(); ++k)
                lista += (k ? ", " : "") + estado.cursos[static_cast<size_t>(faltan[k])].codigo;
            responder_error(res, "Primero debe aprobar: " + lista + ".", 400);
            return;
        }
        estado.inscripciones.push_back({carnet, curso});
        json datos = json::object();
        datos["carnet"] = carnet;
        datos["curso"] = curso;
        responder_json(res, datos, 201);
    });

    svr.Delete(R"(/api/inscripciones/([^/]+)/([^/]+))", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string carnet = req.matches[1].str();
        const std::string curso = req.matches[2].str();
        std::lock_guard<std::mutex> candado(estado.mtx);
        auto& lista = estado.inscripciones;
        const auto it = std::find_if(lista.begin(), lista.end(),
                                     [&](const Inscripcion& x) { return x.carnet == carnet && x.curso == curso; });
        if (it == lista.end()) {
            responder_error(res, "El estudiante no está inscrito en " + curso + ".", 404);
            return;
        }
        lista.erase(it);
        json datos = json::object();
        datos["ok"] = true;
        responder_json(res, datos);
    });
}
