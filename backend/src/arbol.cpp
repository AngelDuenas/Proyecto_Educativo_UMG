// Árbol académico
#include "arbol.hpp"

#include <mutex>
#include <queue>
#include <set>

#include "respuestas.hpp"

namespace {

NodoArbol* crear_nodo(const std::string& nombre, const std::string& tipo, const std::string& codigo = "") {
    NodoArbol* nodo = new NodoArbol();
    nodo->nombre = nombre;
    nodo->tipo = tipo;
    nodo->codigo = codigo;
    return nodo;
}

json nodo_a_json(const NodoArbol* nodo) {
    json j = json::object();
    j["nombre"] = nodo->nombre;
    j["tipo"] = nodo->tipo;
    j["codigo"] = nodo->codigo;
    json hijos = json::array();
    for (const NodoArbol* hijo : nodo->hijos) hijos.push_back(nodo_a_json(hijo));
    j["hijos"] = hijos;
    return j;
}

json recorrido_a_json(const NodoArbol* raiz, const std::string& tipo) {
    std::vector<std::string> nombres;
    if (tipo == "postorden")     recorrido_postorden(raiz, nombres);
    else if (tipo == "niveles")  recorrido_niveles(raiz, nombres);
    else                         recorrido_preorden(raiz, nombres);
    return json(nombres);
}

} 

NodoArbol* construir_arbol(const Estado& e) {
    NodoArbol* raiz = crear_nodo("Universidad Mariano Gálvez", "Universidad");
    NodoArbol* facultad = crear_nodo("Facultad de Ingeniería", "Facultad");
    NodoArbol* carrera = crear_nodo("Ingeniería en Sistemas", "Carrera");
    raiz->hijos.push_back(facultad);
    facultad->hijos.push_back(carrera);

    std::set<int> semestres;
    for (const CursoEstado& c : e.cursos) semestres.insert(c.semestre);
    for (int s : semestres) {
        NodoArbol* semestre = crear_nodo("Semestre " + std::to_string(s), "Semestre");
        for (const CursoEstado& c : e.cursos)
            if (c.semestre == s) semestre->hijos.push_back(crear_nodo(c.nombre, "Curso", c.codigo));
        carrera->hijos.push_back(semestre);
    }
    return raiz;
}

void liberar_arbol(NodoArbol* nodo) {
    if (nodo == nullptr) return;
    for (NodoArbol* hijo : nodo->hijos) liberar_arbol(hijo);
    delete nodo;
}

void recorrido_preorden(const NodoArbol* nodo, std::vector<std::string>& salida) {
    if (nodo == nullptr) return;
    salida.push_back(nodo->nombre);
    for (const NodoArbol* hijo : nodo->hijos) recorrido_preorden(hijo, salida);
}

void recorrido_postorden(const NodoArbol* nodo, std::vector<std::string>& salida) {
    if (nodo == nullptr) return;
    for (const NodoArbol* hijo : nodo->hijos) recorrido_postorden(hijo, salida);
    salida.push_back(nodo->nombre);
}

void recorrido_niveles(const NodoArbol* raiz, std::vector<std::string>& salida) {
    if (raiz == nullptr) return;
    std::queue<const NodoArbol*> cola;
    cola.push(raiz);
    while (!cola.empty()) {
        const NodoArbol* actual = cola.front();
        cola.pop();
        salida.push_back(actual->nombre);
        for (const NodoArbol* hijo : actual->hijos) cola.push(hijo);
    }
}

void registrar_rutas_arbol(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/arbol", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        NodoArbol* raiz = construir_arbol(estado);

        json recorridos = json::object();
        recorridos["preorden"] = recorrido_a_json(raiz, "preorden");
        recorridos["postorden"] = recorrido_a_json(raiz, "postorden");
        recorridos["niveles"] = recorrido_a_json(raiz, "niveles");

        json datos = json::object();
        datos["raiz"] = nodo_a_json(raiz);
        datos["recorrido"] = recorridos["preorden"];
        datos["recorridos"] = recorridos;
        liberar_arbol(raiz);
        responder_json(res, datos);
    });

    svr.Get("/api/arbol/recorrido", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string tipo = req.has_param("tipo") ? req.get_param_value("tipo") : "preorden";
        if (tipo != "preorden" && tipo != "postorden" && tipo != "niveles") {
            responder_error(res, "El recorrido debe ser preorden, postorden o niveles.", 400);
            return;
        }
        std::lock_guard<std::mutex> candado(estado.mtx);
        NodoArbol* raiz = construir_arbol(estado);
        json datos = json::object();
        datos["tipo"] = tipo;
        datos["recorrido"] = recorrido_a_json(raiz, tipo);
        liberar_arbol(raiz);
        responder_json(res, datos);
    });
}
