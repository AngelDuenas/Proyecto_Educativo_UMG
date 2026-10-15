// Módulo Memoria (solo lectura): muestra las matrices TAL COMO ESTÁN en la RAM del servidor,
// sin convertir nada, para comprobar que cada registro cae en su fila y columna.
//   GET /api/memoria -> {
//     capacidad: {estudiantes, cursos, dias, periodos},
//     estudiantes: [{fila, carnet, nombre}], cursos: [{columna, codigo, nombre}],
//     notas: (n+1)×m valores crudos (SIN_NOTA = -1; la última fila es la siguiente libre),
//     asistencia: (n+1)×5 (1 presente, 0 ausente),
//     adyacencia: m×m (1 = prerrequisito),
//     horario: 5×6 listas de códigos,
//     ocupacion: {filasUsadas, columnasUsadas, celdasTotales, notasRegistradas} }
#pragma once
#include "estado.hpp"
#include "httplib.h"

void registrar_rutas_memoria(httplib::Server& svr, Estado& estado);
