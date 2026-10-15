#pragma once
#include <mutex>

#include "estado.hpp"
#include "httplib.h"

// Rutas de docentes, cursos y asignaciones. Une el módulo de Cristian (docentes.cpp)
// con el catálogo de cursos que vive en Estado.
void registrar_rutas_catalogo(httplib::Server& svr, Estado& estado);

// Candado del módulo de docentes (no es seguro entre hilos).
// Orden para no bloquearse: si se necesitan los dos, primero estado.mtx y luego este.
std::mutex& candado_docentes();
