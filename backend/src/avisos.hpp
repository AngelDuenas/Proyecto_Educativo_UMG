// Módulo Avisos / calendario: exámenes, entregas, eventos y avisos generales.
//   GET    /api/avisos            -> [{id, titulo, descripcion, fecha, tipo, curso, cursoNombre}]  ordenados por fecha
//   POST   /api/avisos            {titulo, descripcion?, fecha "AAAA-MM-DD", tipo, curso?}
//   PUT    /api/avisos/:id        mismo cuerpo
//   DELETE /api/avisos/:id
#pragma once
#include "estado.hpp"
#include "httplib.h"
#include "json.hpp"

void cargar_ejemplo_avisos(Estado& e);
nlohmann::json avisos_a_json(const Estado& e);                  // para data/avisos.json
bool avisos_desde_json(const nlohmann::json& j, Estado& e);

void registrar_rutas_avisos(httplib::Server& svr, Estado& estado);
