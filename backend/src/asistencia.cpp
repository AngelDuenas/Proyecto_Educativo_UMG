// DUEÑO: P4 · Tareas: T17
// Módulo Asistencia: matriz asistencia[estudiante][día] y porcentajes.
// Valores: 1 = presente, 0 = ausente, -1 (SIN_REGISTRO) = sin registro.
//
//   GET /api/asistencia
//   PUT /api/asistencia   {estudiante_id, dia (0 a 4), presente (true/false, o null para borrar)}
#include "asistencia.hpp"

#include <algorithm>
#include <mutex>
#include <string>

#include "respuestas.hpp"

namespace {
size_t total_estudiantes(const Estado& e) {
    return std::min(e.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));
}
}  // namespace

// Cuenta los días registrados y cuántos fueron "presente".
// El porcentaje se calcula solo sobre los días registrados: si falta un día por
// registrar, no se le cuenta como ausencia.
bool porcentaje_asistencia(const Estado& e, size_t fila, double& porcentaje) {
    if (fila >= total_estudiantes(e)) return false;
    int registrados = 0, presentes = 0;
    for (int d = 0; d < DIAS_SEMANA; ++d) {
        const int valor = e.asistencia[fila][d];
        if (valor == SIN_REGISTRO) continue;
        ++registrados;
        if (valor == 1) ++presentes;
    }
    if (registrados == 0) return false;
    porcentaje = presentes * 100.0 / registrados;
    return true;
}

void registrar_rutas_asistencia(httplib::Server& svr, Estado& estado) {
    // ---- Matriz completa + porcentajes ----
    svr.Get("/api/asistencia", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t ne = total_estudiantes(estado);

        json nombres = json::array(), ids = json::array(), matriz = json::array(), porcentajes = json::array();
        for (size_t i = 0; i < ne; ++i) {
            nombres.push_back(estado.estudiantes[i].nombre);
            ids.push_back(estado.estudiantes[i].id);

            json fila = json::array();
            for (int d = 0; d < DIAS_SEMANA; ++d) fila.push_back(estado.asistencia[i][d]);
            matriz.push_back(fila);

            double porcentaje = 0.0;
            if (porcentaje_asistencia(estado, i, porcentaje)) porcentajes.push_back(redondear(porcentaje, 1));
            else                                              porcentajes.push_back(nullptr);
        }

        json dias = json::array();
        dias.push_back("L"); dias.push_back("M"); dias.push_back("X"); dias.push_back("J"); dias.push_back("V");

        json datos = json::object();
        datos["estudiantes"] = nombres;
        datos["estudiante_ids"] = ids;
        datos["dias"] = dias;
        datos["matriz"] = matriz;
        datos["porcentajes"] = porcentajes;
        responder_ok(res, datos);
    });

    // ---- Marcar asistencia de un estudiante en un día ----
    svr.Put("/api/asistencia", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;

        if (!cuerpo.contains("estudiante_id") || !cuerpo.at("estudiante_id").is_number_integer()) {
            responder_error(res, "Falta 'estudiante_id' (número entero).", 400);
            return;
        }
        if (!cuerpo.contains("dia") || !cuerpo.at("dia").is_number_integer()) {
            responder_error(res, "Falta 'dia' (número entero de 0 a 4).", 400);
            return;
        }
        const long long dia = cuerpo.at("dia").get<long long>();
        if (dia < 0 || dia >= DIAS_SEMANA) {
            responder_error(res, "El día debe estar entre 0 (lunes) y 4 (viernes).", 400);
            return;
        }
        if (!cuerpo.contains("presente")) {
            responder_error(res, "Falta 'presente' (true, false o null).", 400);
            return;
        }

        int valor = SIN_REGISTRO;
        const json& jp = cuerpo.at("presente");
        if (jp.is_boolean()) {
            valor = jp.get<bool>() ? 1 : 0;
        } else if (jp.is_null()) {
            valor = SIN_REGISTRO;
        } else if (jp.is_number_integer() && (jp.get<long long>() == 0 || jp.get<long long>() == 1)) {
            valor = static_cast<int>(jp.get<long long>());
        } else {
            responder_error(res, "'presente' debe ser true, false o null.", 400);
            return;
        }

        std::lock_guard<std::mutex> candado(estado.mtx);
        const int fila = indice_estudiante_por_id(estado, static_cast<int>(cuerpo.at("estudiante_id").get<long long>()));
        if (fila < 0) {
            responder_error(res, "No existe un estudiante con ese id.", 404);
            return;
        }
        if (fila >= MAX_ESTUDIANTES) {
            responder_error(res, "La posición queda fuera de la matriz de asistencia.", 500);
            return;
        }

        estado.asistencia[fila][dia] = valor;

        json datos = json::object();
        datos["estudiante_id"] = estado.estudiantes[static_cast<size_t>(fila)].id;
        datos["dia"] = dia;
        datos["valor"] = valor;
        double porcentaje = 0.0;
        if (porcentaje_asistencia(estado, static_cast<size_t>(fila), porcentaje)) datos["porcentaje"] = redondear(porcentaje, 1);
        else                                                                       datos["porcentaje"] = nullptr;
        responder_ok(res, datos);
    });
}
