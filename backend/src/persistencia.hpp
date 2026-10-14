// DUEÑO: P2 · Tareas: T24
#pragma once
#include <string>

#include "estado.hpp"
#include "httplib.h"

// Guardan/cargan estudiantes, cursos, notas y asistencia en archivos JSON dentro de `dir`.
// No bloquean el mutex: quien las llame desde un handler debe bloquearlo antes.
bool guardar_todo(Estado& e, const std::string& dir = "data");
bool cargar_todo(Estado& e, const std::string& dir = "data");

void registrar_rutas_persistencia(httplib::Server& svr, Estado& estado);   // POST /api/guardar y /api/cargar
