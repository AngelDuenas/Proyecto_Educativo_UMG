#ifndef ASISTENCIA_HPP
#define ASISTENCIA_HPP

#include <string>

void inicializarAsistencia();
std::string obtenerAsistenciaJSON();
bool actualizarAsistencia(const std::string& carnet, int diaIndex, int valor);

#endif