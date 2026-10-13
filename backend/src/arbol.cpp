// DUEÑO: P3 · Tareas: T12
// BASE MÍNIMA (la completa P3): GET /api/arbol devuelve solo la raíz "Colegio".
#include "arbol.hpp"

#include <mutex>

#include "respuestas.hpp"

namespace {
json nodo_a_json(const Nodo* nodo) {
    json j = json::object();
    j["nombre"] = nodo->nombre;
    json hijos = json::array();
    for (const Nodo* h : nodo->hijos) hijos.push_back(nodo_a_json(h));
    j["hijos"] = hijos;
    return j;
}
}  // namespace

void registrar_rutas_arbol(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/arbol", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        if (estado.raiz_arbol == nullptr) {
            // TODO(P3, T12): construir el árbol semilla en memoria (Colegio -> Básicos / Diversificado -> grados -> secciones)
            json raiz = json::object();
            raiz["nombre"] = "Colegio";
            raiz["hijos"] = json::array();
            responder_ok(res, raiz);
            return;
        }
        responder_ok(res, nodo_a_json(estado.raiz_arbol));
    });

    // TODO(P3, T12): GET /api/arbol/recorrido?tipo=preorden|postorden|niveles
    svr.Get("/api/arbol/recorrido", [](const httplib::Request&, httplib::Response& res) {
        responder_error(res, "No implementado todavía.", 501);
    });
    // TODO(P3, T12): POST /api/arbol/nodo {padre, nombre}
    svr.Post("/api/arbol/nodo", [](const httplib::Request&, httplib::Response& res) {
        responder_error(res, "No implementado todavía.", 501);
    });
}
