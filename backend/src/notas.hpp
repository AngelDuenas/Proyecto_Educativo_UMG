#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_notas(httplib::Server& svr, Estado& estado);
