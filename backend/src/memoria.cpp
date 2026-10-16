#include "memoria.hpp"

#include <algorithm>
#include <mutex>

#include "respuestas.hpp"

void registrar_rutas_memoria(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/memoria", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t n = std::min(estado.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));
        const size_t m = std::min(estado.cursos.size(), static_cast<size_t>(MAX_CURSOS));
        const size_t filas = std::min(n + 1, static_cast<size_t>(MAX_ESTUDIANTES));

        json estudiantes = json::array();
        for (size_t i = 0; i < n; ++i)
            estudiantes.push_back({{"fila", i}, {"carnet", estado.estudiantes[i].carnet}, {"nombre", estado.estudiantes[i].nombre}});

        json cursos = json::array();
        for (size_t j = 0; j < m; ++j)
            cursos.push_back({{"columna", j}, {"codigo", estado.cursos[j].codigo}, {"nombre", estado.cursos[j].nombre}});

        int notas_registradas = 0;
        json notas = json::array();
        json asistencia = json::array();
        for (size_t i = 0; i < filas; ++i) {
            json fila_notas = json::array();
            for (size_t j = 0; j < m; ++j) {
                fila_notas.push_back(estado.notas[i][j]);
                if (i < n && estado.notas[i][j] >= 0.0f) ++notas_registradas;
            }
            notas.push_back(fila_notas);

            json fila_asistencia = json::array();
            for (int d = 0; d < DIAS_SEMANA; ++d) fila_asistencia.push_back(estado.asistencia[i][d]);
            asistencia.push_back(fila_asistencia);
        }

        json adyacencia = json::array();
        for (size_t i = 0; i < m; ++i) {
            json fila = json::array();
            for (size_t j = 0; j < m; ++j) fila.push_back(estado.adyacencia[i][j]);
            adyacencia.push_back(fila);
        }

        json horario = json::array();
        for (int d = 0; d < DIAS_SEMANA; ++d) {
            json dia = json::array();
            for (int p = 0; p < PERIODOS; ++p) dia.push_back(estado.horario[d][p]);
            horario.push_back(dia);
        }

        json datos = json::object();
        datos["capacidad"] = {{"estudiantes", MAX_ESTUDIANTES}, {"cursos", MAX_CURSOS}, {"dias", DIAS_SEMANA}, {"periodos", PERIODOS}};
        datos["estudiantes"] = estudiantes;
        datos["cursos"] = cursos;
        datos["notas"] = notas;
        datos["asistencia"] = asistencia;
        datos["adyacencia"] = adyacencia;
        datos["horario"] = horario;
        datos["ocupacion"] = {{"filasUsadas", n},
                              {"columnasUsadas", m},
                              {"celdasTotales", MAX_ESTUDIANTES * MAX_CURSOS},
                              {"notasRegistradas", notas_registradas}};
        responder_json(res, datos);
    });
}
