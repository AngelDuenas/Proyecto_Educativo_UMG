#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_reportes(httplib::Server& svr, Estado& estado);
