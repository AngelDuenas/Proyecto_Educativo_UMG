// DUEÑO: P3 · Tareas: T21, T26
// BASE MÍNIMA (la completa P3): GET /api/grafo arma vértices, aristas y matriz
// a partir de estado.adyacencia. BFS, DFS y ruta responden 501.
#include "grafo.hpp"

#include <algorithm>
#include <mutex>

#include "respuestas.hpp"

void registrar_rutas_grafo(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/grafo", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t n = std::min(estado.cursos.size(), static_cast<size_t>(MAX_CURSOS));

        json vertices = json::array(), aristas = json::array(), matriz = json::array();
        for (size_t i = 0; i < n; ++i) {
            json v = json::object();
            v["id"] = estado.cursos[i].id;
            v["nombre"] = estado.cursos[i].nombre;
            vertices.push_back(v);

            json fila = json::array();
            for (size_t j = 0; j < n; ++j) {
                fila.push_back(estado.adyacencia[i][j]);
                if (estado.adyacencia[i][j] == 1) {
                    json a = json::object();
                    a["desde"] = estado.cursos[i].id;   // prerrequisito
                    a["hasta"] = estado.cursos[j].id;   // curso que lo requiere
                    aristas.push_back(a);
                }
            }
            matriz.push_back(fila);
        }

        json datos = json::object();
        datos["vertices"] = vertices;
        datos["aristas"] = aristas;
        datos["matriz"] = matriz;
        responder_ok(res, datos);
    });

    // TODO(P3, T21): POST /api/grafo/arista {desde, hasta}
    svr.Post("/api/grafo/arista", [](const httplib::Request&, httplib::Response& res) {
        responder_error(res, "No implementado todavía.", 501);
    });
    // TODO(P3, T26): BFS, DFS y ruta de prerrequisitos
    svr.Get("/api/grafo/bfs", [](const httplib::Request&, httplib::Response& res) {
        responder_error(res, "No implementado todavía.", 501);
    });
    svr.Get("/api/grafo/dfs", [](const httplib::Request&, httplib::Response& res) {
        responder_error(res, "No implementado todavía.", 501);
    });
    svr.Get("/api/grafo/ruta", [](const httplib::Request&, httplib::Response& res) {
        responder_error(res, "No implementado todavía.", 501);
    });
}
