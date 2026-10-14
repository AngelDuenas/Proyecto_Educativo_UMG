#ifndef GRAFO_HPP
#define GRAFO_HPP

// Grafo dirigido de prerrequisitos.
//   Vértice = curso (en el orden de obtenerCursosJSON()).
//   Arista A → B = "A es prerrequisito de B", guardada en una matriz de adyacencia.

#include <string>

std::string obtenerGrafoJSON();   // { "nodos": [{codigo, nombre}], "matriz": [[0,1,...]] }

// Recorrido BFS (anchura) o DFS (profundidad) desde `origen`.
// Devuelve false si el curso no existe. En `salida` deja { "origen", "orden": [codigos] }.
bool recorrerGrafoJSON(const std::string& origen, bool anchura, std::string& salida);

#endif
