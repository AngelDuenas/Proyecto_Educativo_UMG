#pragma once
#include <mutex>

#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_catalogo(httplib::Server& svr, Estado& estado);

std::mutex& candado_docentes();
