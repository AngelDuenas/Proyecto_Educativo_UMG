#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_grafo(httplib::Server& svr, Estado& estado);
