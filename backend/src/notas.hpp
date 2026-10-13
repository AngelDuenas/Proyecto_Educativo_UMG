// DUEÑO: P2 · Tareas: T11, T15, T18
#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_notas(httplib::Server& svr, Estado& estado);

// Promedio de una fila (estudiante) o de una columna (curso) de la matriz de notas.
// Ignoran SIN_NOTA. Devuelven false si no hay ninguna nota (no se puede promediar).
// Las usa también el módulo de reportes (rankings).
bool promedio_estudiante(const Estado& e, size_t fila, double& promedio);
bool promedio_curso(const Estado& e, size_t columna, double& promedio);
