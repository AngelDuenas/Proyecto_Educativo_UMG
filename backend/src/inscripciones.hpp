// Módulo Inscripciones: qué cursos lleva cada estudiante.
//   GET    /api/inscripciones                  -> [{carnet, nombre, curso, cursoNombre}]
//   GET    /api/inscripciones/:carnet          -> {estudiante, cursos:[{codigo, nombre, semestre, creditos, nota, estado, faltan}], creditosInscritos}
//                                                 estado: aprobado | inscrito | disponible | bloqueado
//   POST   /api/inscripciones {carnet, curso}  -> inscribe si aprobó los prerrequisitos (usa el grafo)
//   DELETE /api/inscripciones/:carnet/:curso
#pragma once
#include <string>
#include <vector>

#include "estado.hpp"
#include "httplib.h"
#include "json.hpp"

// Prerrequisitos de `curso` (posición) que el estudiante (fila) todavía no aprueba.
std::vector<int> prerrequisitos_pendientes(const Estado& e, size_t fila, size_t curso);

void cargar_ejemplo_inscripciones(Estado& e);
nlohmann::json inscripciones_a_json(const Estado& e);                       // para data/asignaciones_estudiantes.json
bool inscripciones_desde_json(const nlohmann::json& j, Estado& e);          // ignora carnets o cursos que ya no existen

void registrar_rutas_inscripciones(httplib::Server& svr, Estado& estado);
