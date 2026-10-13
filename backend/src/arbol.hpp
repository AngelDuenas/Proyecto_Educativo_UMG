// DUEÑO: P3 · Tareas: T12
#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_arbol(httplib::Server& svr, Estado& estado);
