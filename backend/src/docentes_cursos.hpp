// DUEÑO: P5 · Tareas: T14
#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_docentes_cursos(httplib::Server& svr, Estado& estado);
