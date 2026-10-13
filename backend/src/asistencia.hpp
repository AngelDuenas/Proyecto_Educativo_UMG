// DUEÑO: P4 · Tareas: T17
#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_asistencia(httplib::Server& svr, Estado& estado);

// Porcentaje de asistencia de una fila (estudiante), sobre los días que SÍ tienen registro.
// Devuelve false si el estudiante no tiene ningún día registrado.
bool porcentaje_asistencia(const Estado& e, size_t fila, double& porcentaje);
