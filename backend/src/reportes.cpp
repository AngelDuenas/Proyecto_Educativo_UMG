#include "reportes.hpp"

#include <mutex>

#include "algoritmos.hpp"
#include "respuestas.hpp"

void registrar_rutas_reportes(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/reportes", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);

        json ranking_notas = json::array();
        for (size_t i : ranking_por_promedio(estado)) {
            double p = 0.0;
            promedio_estudiante(estado, i, p);
            json o = json::object();
            o["carnet"] = estado.estudiantes[i].carnet;
            o["nombre"] = estado.estudiantes[i].nombre;
            o["promedio"] = redondear(p);
            ranking_notas.push_back(o);
        }

        json ranking_asistencia = json::array();
        for (size_t i : ranking_por_asistencia(estado)) {
            json o = json::object();
            o["carnet"] = estado.estudiantes[i].carnet;
            o["nombre"] = estado.estudiantes[i].nombre;
            o["porcentaje"] = porcentaje_estudiante(estado, i);
            ranking_asistencia.push_back(o);
        }

        json datos = json::object();
        datos["rankingNotas"] = ranking_notas;
        datos["rankingAsistencia"] = ranking_asistencia;
        responder_json(res, datos);
    });
}
