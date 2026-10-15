// DUEÑO: P4 · Módulo Asistencia — formato definido por vista-asistencia.js
//   GET /api/asistencia -> {dias, estudiantes:[{carnet,nombre}], matriz:[[1,1,0,1,1]], porEstudiante:[%], porDia:[%]}
//   PUT /api/asistencia {carnet, dia (0 a 4), valor (1 presente / 0 ausente)}
#include "asistencia.hpp"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>

#include "algoritmos.hpp"
#include "respuestas.hpp"

void registrar_rutas_asistencia(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/asistencia", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t ne = std::min(estado.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));

        json dias = json::array(), estudiantes = json::array(), matriz = json::array();
        json por_estudiante = json::array(), por_dia = json::array();
        dias.push_back("L"); dias.push_back("M"); dias.push_back("X"); dias.push_back("J"); dias.push_back("V");

        for (size_t i = 0; i < ne; ++i) {
            json f = json::object();
            f["carnet"] = estado.estudiantes[i].carnet;
            f["nombre"] = estado.estudiantes[i].nombre;
            estudiantes.push_back(f);

            json fila = json::array();
            for (int d = 0; d < DIAS_SEMANA; ++d) fila.push_back(estado.asistencia[i][d]);
            matriz.push_back(fila);

            por_estudiante.push_back(porcentaje_estudiante(estado, i));   // % de la FILA
        }
        for (int d = 0; d < DIAS_SEMANA; ++d) por_dia.push_back(porcentaje_dia(estado, d));   // % de la COLUMNA

        json datos = json::object();
        datos["dias"] = dias;
        datos["estudiantes"] = estudiantes;
        datos["matriz"] = matriz;
        datos["porEstudiante"] = por_estudiante;
        datos["porDia"] = por_dia;
        responder_json(res, datos);
    });

    svr.Put("/api/asistencia", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;

        if (!cuerpo.contains("carnet") || !cuerpo.at("carnet").is_string()) {
            responder_error(res, "Falta el carnet del estudiante.", 400);
            return;
        }
        if (!cuerpo.contains("dia") || !cuerpo.at("dia").is_number_integer()) {
            responder_error(res, "El día debe ser un número entero de 0 (lunes) a 4 (viernes).", 400);
            return;
        }
        const long long dia = cuerpo.at("dia").get<long long>();
        if (dia < 0 || dia >= DIAS_SEMANA) {
            responder_error(res, "El día debe estar entre 0 (lunes) y 4 (viernes).", 400);
            return;
        }

        // `valor` puede ser 1/0 o true/false; cualquier otra cosa cuenta como ausente (igual que el frontend).
        int valor = 0;
        if (cuerpo.contains("valor")) {
            const json& v = cuerpo.at("valor");
            if (v.is_boolean())      valor = v.get<bool>() ? 1 : 0;
            else if (v.is_number())  valor = (v.get<double>() != 0.0) ? 1 : 0;
        }

        const std::string carnet = cuerpo.at("carnet").get<std::string>();
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int fila = indice_estudiante(estado, carnet);
        if (fila < 0) {
            responder_error(res, "No existe el estudiante " + carnet + ".", 404);
            return;
        }
        estado.asistencia[fila][dia] = valor;
        std::cout << "[matriz] asistencia[" << fila << "][" << dia << "] = " << valor
                  << "   (" << carnet << (valor ? ", presente" : ", ausente") << ")" << std::endl;

        json datos = json::object();
        datos["carnet"] = carnet;
        datos["dia"] = dia;
        datos["valor"] = valor;
        responder_json(res, datos);
    });
}
