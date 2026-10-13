// DUEÑO: P4 · Tareas: T25
// BASE MÍNIMA (la completa P4): devuelven listas vacías.
// Pista: para el ranking pueden usar promedio_estudiante() de notas.hpp y
// porcentaje_asistencia() de asistencia.hpp.
#include "reportes.hpp"

#include "respuestas.hpp"

void registrar_rutas_reportes(httplib::Server& svr, Estado& /*estado*/) {
    // TODO(P4, T25): estudiantes ordenados por promedio (descendente)
    svr.Get("/api/reportes/ranking", [](const httplib::Request&, httplib::Response& res) {
        responder_ok(res, json::array());
    });
    // TODO(P4, T25): ranking de asistencia
    svr.Get("/api/reportes/asistencia", [](const httplib::Request&, httplib::Response& res) {
        responder_ok(res, json::array());
    });
    // TODO(P4, T25): cursos relacionados usando el BFS del módulo grafo (P3)
    svr.Get("/api/reportes/cursos-relacionados", [](const httplib::Request&, httplib::Response& res) {
        responder_ok(res, json::array());
    });
}
