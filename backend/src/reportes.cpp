#include "reportes.hpp"

#include <algorithm>
#include <map>

#include "../third_party/json.hpp"
#include "asistencia.hpp"
#include "estudiantes.hpp"
#include "notas.hpp"

using json = nlohmann::json;

// Los módulos devuelven texto JSON; aquí se interpreta para combinarlos.
static json leer(const std::string& texto) {
    json j = json::parse(texto, nullptr, false);
    return j.is_discarded() ? json() : j;
}

std::string obtenerReportesJSON() {
    // carnet → nombre
    std::map<std::string, std::string> nombres;
    json estudiantes = leer(obtenerEstudiantesJSON("", ""));
    if (estudiantes.is_array())
        for (const auto& e : estudiantes) nombres[e.value("carnet", "")] = e.value("nombre", "");

    auto nombreDe = [&nombres](const std::string& carnet) {
        auto it = nombres.find(carnet);
        return it != nombres.end() ? it->second : carnet;
    };

    // Ranking por promedio (se omiten estudiantes sin notas)
    json rankingNotas = json::array();
    json stats = leer(obtenerEstadisticasJSON());
    if (stats.contains("porEstudiante") && stats["porEstudiante"].is_array()) {
        for (const auto& p : stats["porEstudiante"]) {
            if (!p.contains("promedio") || !p["promedio"].is_number()) continue;
            const std::string carnet = p.value("carnet", "");
            rankingNotas.push_back({{"carnet", carnet}, {"nombre", nombreDe(carnet)}, {"promedio", p["promedio"]}});
        }
    }
    std::stable_sort(rankingNotas.begin(), rankingNotas.end(), [](const json& a, const json& b) {
        return a["promedio"].get<double>() > b["promedio"].get<double>();
    });

    // Ranking por asistencia
    json rankingAsistencia = json::array();
    json asis = leer(obtenerAsistenciaJSON());
    if (asis.contains("estudiantes") && asis.contains("porEstudiante")) {
        const json& lista = asis["estudiantes"];
        const json& porcentajes = asis["porEstudiante"];
        for (size_t i = 0; i < lista.size() && i < porcentajes.size(); i++) {
            const std::string carnet = lista[i].value("carnet", "");
            rankingAsistencia.push_back({{"carnet", carnet},
                                         {"nombre", lista[i].value("nombre", nombreDe(carnet))},
                                         {"porcentaje", porcentajes[i]}});
        }
    }
    std::stable_sort(rankingAsistencia.begin(), rankingAsistencia.end(), [](const json& a, const json& b) {
        return a["porcentaje"].get<double>() > b["porcentaje"].get<double>();
    });

    return json{{"rankingNotas", rankingNotas}, {"rankingAsistencia", rankingAsistencia}}.dump();
}
