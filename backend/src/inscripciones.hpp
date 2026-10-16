#pragma once
#include <string>
#include <vector>

#include "estado.hpp"
#include "httplib.h"
#include "json.hpp"

std::vector<int> prerrequisitos_pendientes(const Estado& e, size_t fila, size_t curso);

void cargar_ejemplo_inscripciones(Estado& e);
nlohmann::json inscripciones_a_json(const Estado& e);                       
bool inscripciones_desde_json(const nlohmann::json& j, Estado& e);          

void registrar_rutas_inscripciones(httplib::Server& svr, Estado& estado);
