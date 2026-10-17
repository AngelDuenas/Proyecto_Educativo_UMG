#pragma once
#include "estado.hpp"
#include "httplib.h"
#include "json.hpp"

void cargar_ejemplo_horario(Estado& e);
nlohmann::json horario_a_json(const Estado& e);                  
bool horario_desde_json(const nlohmann::json& j, Estado& e);     

void registrar_rutas_horario(httplib::Server& svr, Estado& estado);
