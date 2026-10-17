// DUEÑO: algoritmos sobre las matrices y el grafo
#include "algoritmos.hpp"

#include <algorithm>
#include <cmath>

double redondear(double valor, int decimales) {
    const double factor = std::pow(10.0, decimales);
    return std::round(valor * factor) / factor;
}

namespace {
size_t filas(const Estado& e)    { return std::min(e.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES)); }
size_t columnas(const Estado& e) { return std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS)); }
bool hay_nota(float n)           { return n >= 0.0f; }  
} 

bool promedio_estudiante(const Estado& e, size_t fila, double& promedio) {
    if (fila >= filas(e)) return false;
    double suma = 0.0;
    int cantidad = 0;
    for (size_t j = 0; j < columnas(e); ++j) {
        if (hay_nota(e.notas[fila][j])) { suma += e.notas[fila][j]; ++cantidad; } 
    }
    if (cantidad == 0) return false;
    promedio = suma / cantidad;
    return true;
}

// Recorre la COLUMNA del curso sumando solo las notas que existen.
bool promedio_curso(const Estado& e, size_t columna, double& promedio) {
    if (columna >= columnas(e)) return false;
    double suma = 0.0;
    int cantidad = 0;
    for (size_t i = 0; i < filas(e); ++i) {
        if (hay_nota(e.notas[i][columna])) { suma += e.notas[i][columna]; ++cantidad; }
    }
    if (cantidad == 0) return false;
    promedio = suma / cantidad;
    return true;
}

bool promedio_general(const Estado& e, double& promedio) {
    double suma = 0.0;
    int cantidad = 0;
    for (size_t i = 0; i < filas(e); ++i)
        for (size_t j = 0; j < columnas(e); ++j)
            if (hay_nota(e.notas[i][j])) { suma += e.notas[i][j]; ++cantidad; }
    if (cantidad == 0) return false;
    promedio = suma / cantidad;
    return true;
}

// Recorre toda la matriz (fila por fila) guardando la nota más alta y la más baja.
void extremos_notas(const Estado& e, Extremo& mayor, Extremo& menor) {
    mayor = Extremo();
    menor = Extremo();
    for (size_t i = 0; i < filas(e); ++i) {
        for (size_t j = 0; j < columnas(e); ++j) {
            if (!hay_nota(e.notas[i][j])) continue;
            const double n = e.notas[i][j];
            if (!mayor.existe || n > mayor.nota) { mayor.existe = true; mayor.nota = n; mayor.carnet = e.estudiantes[i].carnet; mayor.curso = e.cursos[j].codigo; }
            if (!menor.existe || n < menor.nota) { menor.existe = true; menor.nota = n; menor.carnet = e.estudiantes[i].carnet; menor.curso = e.cursos[j].codigo; }
        }
    }
}

void contar_aprobados(const Estado& e, int& aprobados, int& reprobados) {
    aprobados = 0;
    reprobados = 0;
    for (size_t i = 0; i < filas(e); ++i)
        for (size_t j = 0; j < columnas(e); ++j)
            if (hay_nota(e.notas[i][j])) {
                if (e.notas[i][j] >= NOTA_APROBACION) ++aprobados;
                else                                  ++reprobados;
            }
}

double porcentaje_estudiante(const Estado& e, size_t fila) {
    if (fila >= filas(e)) return 0.0;
    int presentes = 0;
    for (int d = 0; d < DIAS_SEMANA; ++d) presentes += (e.asistencia[fila][d] == 1) ? 1 : 0;
    return redondear(presentes * 100.0 / DIAS_SEMANA, 1);
}

double porcentaje_dia(const Estado& e, int dia) {
    if (dia < 0 || dia >= DIAS_SEMANA || filas(e) == 0) return 0.0;
    int presentes = 0;
    for (size_t i = 0; i < filas(e); ++i) presentes += (e.asistencia[i][dia] == 1) ? 1 : 0;
    return redondear(presentes * 100.0 / static_cast<double>(filas(e)), 1);
}

namespace {
void ordenar_desc(std::vector<size_t>& idx, const std::vector<double>& valor) {
    for (size_t i = 1; i < idx.size(); ++i) {
        const size_t clave = idx[i];
        size_t j = i;
        while (j > 0 && valor[idx[j - 1]] < valor[clave]) { idx[j] = idx[j - 1]; --j; }
        idx[j] = clave;
    }
}
} 

std::vector<size_t> ranking_por_promedio(const Estado& e) {
    std::vector<double> valor(filas(e), 0.0);
    std::vector<size_t> idx;
    for (size_t i = 0; i < filas(e); ++i) {
        double p = 0.0;
        if (promedio_estudiante(e, i, p)) { valor[i] = p; idx.push_back(i); }
    }
    ordenar_desc(idx, valor);
    return idx;
}

std::vector<size_t> ranking_por_asistencia(const Estado& e) {
    std::vector<double> valor(filas(e), 0.0);
    std::vector<size_t> idx;
    for (size_t i = 0; i < filas(e); ++i) { valor[i] = porcentaje_estudiante(e, i); idx.push_back(i); }
    ordenar_desc(idx, valor);
    return idx;
}

std::vector<int> recorrido_bfs(const Estado& e, int origen) {
    const int n = static_cast<int>(columnas(e));
    std::vector<int> orden;
    if (origen < 0 || origen >= n) return orden;
    std::vector<bool> visitado(static_cast<size_t>(n), false);
    std::vector<int> cola;
    size_t cabeza = 0;
    cola.push_back(origen);
    visitado[static_cast<size_t>(origen)] = true;
    while (cabeza < cola.size()) {
        const int actual = cola[cabeza++];
        orden.push_back(actual);
        for (int j = 0; j < n; ++j) {
            if (e.adyacencia[actual][j] == 1 && !visitado[static_cast<size_t>(j)]) {
                visitado[static_cast<size_t>(j)] = true;
                cola.push_back(j);
            }
        }
    }
    return orden;
}

namespace {
void dfs_visitar(const Estado& e, int actual, int n, std::vector<bool>& visitado, std::vector<int>& orden) {
    visitado[static_cast<size_t>(actual)] = true;
    orden.push_back(actual);
    for (int j = 0; j < n; ++j)
        if (e.adyacencia[actual][j] == 1 && !visitado[static_cast<size_t>(j)]) dfs_visitar(e, j, n, visitado, orden);
}
}

// DFS: baja lo más profundo posible por un camino antes de probar el siguiente.
std::vector<int> recorrido_dfs(const Estado& e, int origen) {
    const int n = static_cast<int>(columnas(e));
    std::vector<int> orden;
    if (origen < 0 || origen >= n) return orden;
    std::vector<bool> visitado(static_cast<size_t>(n), false);
    dfs_visitar(e, origen, n, visitado, orden);
    return orden;
}
