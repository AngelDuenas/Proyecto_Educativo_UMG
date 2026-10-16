#pragma once
#include <string>
#include <vector>

#include "estado.hpp"
#include "httplib.h"

struct NodoArbol {
    std::string nombre;
    std::string tipo;
    std::string codigo;              
    std::vector<NodoArbol*> hijos;
};

NodoArbol* construir_arbol(const Estado& e);   
void liberar_arbol(NodoArbol* nodo);

void recorrido_preorden(const NodoArbol* nodo, std::vector<std::string>& salida);   
void recorrido_postorden(const NodoArbol* nodo, std::vector<std::string>& salida);  
void recorrido_niveles(const NodoArbol* raiz, std::vector<std::string>& salida);   

void registrar_rutas_arbol(httplib::Server& svr, Estado& estado);
