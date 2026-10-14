#pragma once
#include "estado.hpp"
#include "httplib.h"

// Rutas de docentes, cursos, asignaciones y árbol. Une el módulo de Cristian
// (docentes.cpp / arbol.cpp) con el catálogo de cursos que vive en Estado.
void registrar_rutas_catalogo(httplib::Server& svr, Estado& estado);
