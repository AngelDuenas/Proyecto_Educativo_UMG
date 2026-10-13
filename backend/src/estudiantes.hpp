// DUEÑO: P1 · Tareas: T10
#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_estudiantes(httplib::Server& svr, Estado& estado);
