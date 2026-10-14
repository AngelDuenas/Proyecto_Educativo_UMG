#ifndef REPORTES_HPP
#define REPORTES_HPP

#include <string>

// { "rankingNotas": [{carnet, nombre, promedio}], "rankingAsistencia": [{carnet, nombre, porcentaje}] }
// Ambos ordenados de mayor a menor. Usa los datos de notas, asistencia y estudiantes.
std::string obtenerReportesJSON();

#endif
