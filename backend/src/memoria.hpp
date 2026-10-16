#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_memoria(httplib::Server& svr, Estado& estado);
