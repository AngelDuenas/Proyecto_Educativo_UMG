// Módulo Horarios: matriz horario[día][periodo] con los cursos de cada bloque.
//   GET /api/horario[?carnet=X]  -> {dias, periodos:[{inicio, fin}], celdas[d][p]:[{codigo, nombre, semestre, docentes}],
//                                    choques:[{dia, periodo, tipo, quien, cursos}], bloquesPorCurso:[{codigo, nombre, creditos, bloques}]}
//                                   con ?carnet= solo muestra los cursos en los que ese estudiante está inscrito.
//   PUT /api/horario {dia, periodo, cursos:[codigos]}   -> reemplaza el bloque (máximo MAX_CURSOS_POR_BLOQUE)
// Un CHOQUE es un docente o un estudiante con dos cursos en el mismo bloque.
#pragma once
#include "estado.hpp"
#include "httplib.h"
#include "json.hpp"

void cargar_ejemplo_horario(Estado& e);
nlohmann::json horario_a_json(const Estado& e);                  // para data/horario.json
bool horario_desde_json(const nlohmann::json& j, Estado& e);     // ignora códigos de cursos que ya no existen

void registrar_rutas_horario(httplib::Server& svr, Estado& estado);
