// DUEÑO: P3 (base mínima hecha por P1 para que la vista funcione; P3 puede mejorarla o reemplazarla)
//   GET /api/grafo                      -> {nodos:[{codigo,nombre}], matriz:[[0,1,...]]}
//   GET /api/grafo/bfs?origen=MAT101    -> {origen, orden:["MAT101","MAT102",...]}
//   GET /api/grafo/dfs?origen=MAT101    -> igual, en profundidad
#include "grafo.hpp"

#include <algorithm>
#include <mutex>

#include "algoritmos.hpp"
#include "respuestas.hpp"

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
}
