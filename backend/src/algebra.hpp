// Módulo Álgebra lineal aplicada (de MUESTRA, solo lectura).
// Explica con los datos reales del sistema cómo se usan las operaciones del curso:
//   GET /api/algebra -> {
//     estudiantes, cursos, A (notas n×m, null = sin nota), transpuesta (m×n),
//     creditos (vector c), productoAc (A·c), creditosConNota, promedioPonderado, promedioSimple,
//     promedioCursos ((1/n)·Aᵀ·1), combinacion {pesoNotas, pesoAsistencia, asistencia, resultado},
//     adyacencia {codigos, M, M2 (M·M), cadenas: [{desde, pasaPor, hasta}]} }
#pragma once
#include <vector>

#include "estado.hpp"
#include "httplib.h"

using Matriz = std::vector<std::vector<double>>;

Matriz transponer(const Matriz& a);                                   // (Aᵀ)[j][i] = A[i][j]
Matriz multiplicar(const Matriz& a, const Matriz& b);                 // (A·B)[i][j] = Σk A[i][k]·B[k][j]
std::vector<double> por_vector(const Matriz& a, const std::vector<double>& v);   // (A·v)[i] = Σj A[i][j]·v[j]

void registrar_rutas_algebra(httplib::Server& svr, Estado& estado);
