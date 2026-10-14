#ifndef ARBOL_HPP
#define ARBOL_HPP

#include <string>

struct NodoArbol {
    std::string nombre;
    std::string tipo;
    std::string codigo;
    NodoArbol* hijos[10]; 
    int totalHijos;
};

std::string obtenerArbolJSON();

#endif