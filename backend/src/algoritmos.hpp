// DUEÑO: P1/P2/P4 · algoritmos sobre las matrices y el grafo (sin librerías externas)
#pragma once
#include <string>
#include <vector>

#include "estado.hpp"

double redondear(double valor, int decimales = 2);

// ---- Notas (matriz notas[estudiante][curso]) ----
bool promedio_estudiante(const Estado& e, size_t fila, double& promedio);     // promedio de una FILA
bool promedio_curso(const Estado& e, size_t columna, double& promedio);       // promedio de una COLUMNA
bool promedio_general(const Estado& e, double& promedio);                     // de todas las notas
struct Extremo { bool existe = false; double nota = 0.0; std::string carnet; std::string curso; };
void extremos_notas(const Estado& e, Extremo& mayor, Extremo& menor);
void contar_aprobados(const Estado& e, int& aprobados, int& reprobados);      // cuenta NOTAS (no estudiantes)

// ---- Asistencia (matriz asistencia[estudiante][día]) ----
double porcentaje_estudiante(const Estado& e, size_t fila);                   // % de una FILA (1 decimal)
double porcentaje_dia(const Estado& e, int dia);                              // % de una COLUMNA (1 decimal)

// ---- Rankings (índices de estudiantes, de mayor a menor) ----
std::vector<size_t> ranking_por_promedio(const Estado& e);
std::vector<size_t> ranking_por_asistencia(const Estado& e);

// ---- Grafo de prerrequisitos (matriz de adyacencia) ----
std::vector<int> recorrido_bfs(const Estado& e, int origen);   // por niveles, con cola
std::vector<int> recorrido_dfs(const Estado& e, int origen);   // en profundidad, recursivo
