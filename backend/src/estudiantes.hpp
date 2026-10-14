#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_estudiantes(httplib::Server& svr, Estado& estado);
