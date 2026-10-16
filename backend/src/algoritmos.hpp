#pragma once
#include <string>
#include <vector>

#include "estado.hpp"

double redondear(double valor, int decimales = 2);

bool promedio_estudiante(const Estado& e, size_t fila, double& promedio);     
bool promedio_curso(const Estado& e, size_t columna, double& promedio);      
bool promedio_general(const Estado& e, double& promedio);                     
struct Extremo { bool existe = false; double nota = 0.0; std::string carnet; std::string curso; };
void extremos_notas(const Estado& e, Extremo& mayor, Extremo& menor);
void contar_aprobados(const Estado& e, int& aprobados, int& reprobados);      

double porcentaje_estudiante(const Estado& e, size_t fila);                  
double porcentaje_dia(const Estado& e, int dia);                             

std::vector<size_t> ranking_por_promedio(const Estado& e);
std::vector<size_t> ranking_por_asistencia(const Estado& e);

std::vector<int> recorrido_bfs(const Estado& e, int origen);   
std::vector<int> recorrido_dfs(const Estado& e, int origen);   
