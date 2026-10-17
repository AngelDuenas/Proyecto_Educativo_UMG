#include "grafo.hpp"

#include <algorithm>
#include <mutex>
#include <queue>

#include "algoritmos.hpp"
#include "respuestas.hpp"

namespace {

size_t total_vertices(const Estado& e) { return std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS)); }

std::vector<int> buscar_ruta(const Estado& e, int desde, int hasta) {
    const size_t n = total_vertices(e);
    std::vector<int> padre(n, -1);
    std::vector<bool> visitado(n, false);
    std::queue<int> cola;
    cola.push(desde);
    visitado[static_cast<size_t>(desde)] = true;
    while (!cola.empty()) {
        const int actual = cola.front();
        cola.pop();
        if (actual == hasta) break;
        for (size_t j = 0; j < n; ++j) {
            if (e.adyacencia[actual][j] == 1 && !visitado[j]) {
                visitado[j] = true;
                padre[j] = actual;
                cola.push(static_cast<int>(j));
            }
        }
    }
    std::vector<int> ruta;
    if (!visitado[static_cast<size_t>(hasta)]) return ruta;   // no hay camino
    for (int v = hasta; v != -1; v = padre[static_cast<size_t>(v)]) ruta.push_back(v);
    std::reverse(ruta.begin(), ruta.end());
    return ruta;
}

void registrar_rutas_grafo(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/grafo", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t n = std::min(estado.cursos.size(), static_cast<size_t>(MAX_CURSOS));

        json nodos = json::array(), matriz = json::array();
        for (size_t i = 0; i < n; ++i) {
            json v = json::object();
            v["codigo"] = estado.cursos[i].codigo;
            v["nombre"] = estado.cursos[i].nombre;
            nodos.push_back(v);

            json fila = json::array();
            for (size_t j = 0; j < n; ++j) fila.push_back(estado.adyacencia[i][j]);
            matriz.push_back(fila);
        }
        json datos = json::object();
        datos["nodos"] = nodos;
        datos["matriz"] = matriz;
        responder_json(res, datos);
    });

    svr.Get(R"(/api/grafo/(bfs|dfs))", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string tipo = req.matches[1].str();
        const std::string origen = req.get_param_value("origen");

        std::lock_guard<std::mutex> candado(estado.mtx);
        const int inicio = indice_curso(estado, origen);
        if (inicio < 0) {
            responder_error(res, "No existe el curso " + origen + ".", 404);
            return;
        }
        const std::vector<int> recorrido = (tipo == "bfs") ? recorrido_bfs(estado, inicio) : recorrido_dfs(estado, inicio);

        json orden = json::array();
        for (int idx : recorrido) orden.push_back(estado.cursos[static_cast<size_t>(idx)].codigo);
        json datos = json::object();
        datos["origen"] = origen;
        datos["orden"] = orden;
        responder_json(res, datos);
    });

    svr.Get("/api/grafo/ruta", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string desde = req.get_param_value("desde");
        const std::string hasta = req.get_param_value("hasta");
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int i = indice_curso(estado, desde);
        const int j = indice_curso(estado, hasta);
        if (i < 0 || j < 0) {
            responder_error(res, "No existe el curso " + (i < 0 ? desde : hasta) + ".", 404);
            return;
        }
        json ruta = json::array();
        for (int v : buscar_ruta(estado, i, j)) ruta.push_back(estado.cursos[static_cast<size_t>(v)].codigo);
        json datos = json::object();
        datos["desde"] = desde;
        datos["hasta"] = hasta;
        datos["existe"] = !ruta.empty();
        datos["ruta"] = ruta;
        responder_json(res, datos);
    });
}
