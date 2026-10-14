#include "arbol.hpp"
#include <vector>

static void recorrerPreorden(NodoArbol* nodo, std::vector<std::string>& recorrido) {
    if (nodo == nullptr) return;
    recorrido.push_back(nodo->nombre);
    for (int i = 0; i < nodo->totalHijos; i++) {
        recorrerPreorden(nodo->hijos[i], recorrido);
    }
}

static std::string nodoAJSON(NodoArbol* nodo) {
    std::string r = "{\"nombre\":\"" + nodo->nombre + "\",\"tipo\":\"" + nodo->tipo + "\",\"codigo\":\"" + nodo->codigo + "\",\"hijos\":[";
    for (int i = 0; i < nodo->totalHijos; i++) {
        r += nodoAJSON(nodo->hijos[i]);
        if (i < nodo->totalHijos - 1) r += ",";
    }
    r += "]}";
    return r;
}

std::string obtenerArbolJSON() {
    NodoArbol curso1 = {"Matematica I", "Curso", "MAT101", {}, 0};
    NodoArbol curso2 = {"Programacion I", "Curso", "PRG101", {}, 0};
    
    NodoArbol facultad = {"Facultad de Ingenieria", "Facultad", "", {&curso1, &curso2}, 2};
    NodoArbol raiz = {"Universidad Mariano Galvez", "Universidad", "", {&facultad}, 1};

    std::vector<std::string> listaRecorrido;
    recorrerPreorden(&raiz, listaRecorrido);

    std::string r = "{\"raiz\":" + nodoAJSON(&raiz) + ",\"recorrido\":[";
    for (size_t i = 0; i < listaRecorrido.size(); i++) {
        r += "\"" + listaRecorrido[i] + "\"";
        if (i < listaRecorrido.size() - 1) r += ",";
    }
    r += "]}";

    return r;
}