// DUEÑO: P3 · Árbol académico (n-ario)
//   Universidad → Facultad → Carrera → Semestre N → Cursos   (se arma con los cursos reales del Estado)
//   GET  /api/arbol                                   -> {raiz, recorrido (preorden), recorridos:{preorden, postorden, niveles}}
//   GET  /api/arbol/recorrido?tipo=preorden|postorden|niveles -> {tipo, recorrido}
// Es una vista de MUESTRA (solo lectura): explica estructuras de árbol con los datos reales del sistema.
#pragma once
#include <string>
#include <vector>

#include "estado.hpp"
#include "httplib.h"

struct NodoArbol {
    std::string nombre;
    std::string tipo;
    std::string codigo;              // solo los cursos tienen código
    std::vector<NodoArbol*> hijos;
};

NodoArbol* construir_arbol(const Estado& e);   // quien lo pide debe liberarlo con liberar_arbol
void liberar_arbol(NodoArbol* nodo);

void recorrido_preorden(const NodoArbol* nodo, std::vector<std::string>& salida);   // raíz, luego hijos
void recorrido_postorden(const NodoArbol* nodo, std::vector<std::string>& salida);  // hijos, luego raíz
void recorrido_niveles(const NodoArbol* raiz, std::vector<std::string>& salida);    // por niveles, con cola

void registrar_rutas_arbol(httplib::Server& svr, Estado& estado);
