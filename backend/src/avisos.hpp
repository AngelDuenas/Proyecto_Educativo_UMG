#pragma once
#include "estado.hpp"
#include "httplib.h"
#include "json.hpp"

void cargar_ejemplo_avisos(Estado& e);
nlohmann::json avisos_a_json(const Estado& e);                 
bool avisos_desde_json(const nlohmann::json& j, Estado& e);

void registrar_rutas_avisos(httplib::Server& svr, Estado& estado);
