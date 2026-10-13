// DUEÑO: P3 · Tareas: T21, T26
#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_grafo(httplib::Server& svr, Estado& estado);
