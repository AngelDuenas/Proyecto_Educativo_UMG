#ifndef NOTAS_HPP
#define NOTAS_HPP

#include <string>


void inicializarNotas();
std::string obtenerNotasJSON();
std::string obtenerEstadisticasJSON();
bool actualizarNota(const std::string& carnet, const std::string& cursoCodigo, float* nuevaNota);

#endif