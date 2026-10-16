// DUEÑO: P2 · Tareas: T24
#pragma once
#include <string>

#include "estado.hpp"
#include "httplib.h"

bool guardar_todo(Estado& e, const std::string& dir = "data");
bool cargar_todo(Estado& e, const std::string& dir = "data");

void registrar_rutas_persistencia(httplib::Server& svr, Estado& estado);  
