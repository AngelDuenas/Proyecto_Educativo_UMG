#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_asistencia(httplib::Server& svr, Estado& estado);
