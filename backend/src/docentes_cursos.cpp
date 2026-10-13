// DUEÑO: P5 · Tareas: T14
// BASE MÍNIMA (la completa P5): los GET funcionan; lo demás responde 501.
#include "docentes_cursos.hpp"

#include <mutex>

#include "respuestas.hpp"

namespace {
void no_implementado(httplib::Response& res) {
    responder_error(res, "No implementado todavía.", 501);
}
}  // namespace

void registrar_rutas_docentes_cursos(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/docentes", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        json arreglo = json::array();
        for (const Docente& d : estado.docentes) {
            json j = json::object();
            j["id"] = d.id; j["nombre"] = d.nombre; j["especialidad"] = d.especialidad;
            arreglo.push_back(j);
        }
        responder_ok(res, arreglo);
    });

    svr.Get("/api/cursos", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        json arreglo = json::array();
        for (const Curso& c : estado.cursos) {
            json j = json::object();
            j["id"] = c.id; j["nombre"] = c.nombre; j["docente_id"] = c.docente_id;
            arreglo.push_back(j);
        }
        responder_ok(res, arreglo);
    });

    // TODO(P5, T14): registrar docentes y cursos, eliminar y asignar docente a curso (máximo MAX_CURSOS)
    svr.Post("/api/docentes", [](const httplib::Request&, httplib::Response& res) { no_implementado(res); });
    svr.Delete(R"(/api/docentes/(\d{1,9}))", [](const httplib::Request&, httplib::Response& res) { no_implementado(res); });
    svr.Post("/api/cursos", [](const httplib::Request&, httplib::Response& res) { no_implementado(res); });
    svr.Delete(R"(/api/cursos/(\d{1,9}))", [](const httplib::Request&, httplib::Response& res) { no_implementado(res); });
    svr.Post("/api/asignaciones", [](const httplib::Request&, httplib::Response& res) { no_implementado(res); });
}
