// DUEÑO: P5 + P1 · Tareas: T08
#include "estado.hpp"

#include <algorithm>

void inicializar_matrices(Estado& e) {
    for (int i = 0; i < MAX_ESTUDIANTES; ++i) {
        for (int j = 0; j < MAX_CURSOS; ++j)   e.notas[i][j] = SIN_NOTA;
        for (int d = 0; d < DIAS_SEMANA; ++d)  e.asistencia[i][d] = SIN_REGISTRO;
    }
    for (int i = 0; i < MAX_CURSOS; ++i) {
        for (int j = 0; j < MAX_CURSOS; ++j)   e.adyacencia[i][j] = 0;
    }
}

namespace {
// Deja una fila de las matrices de notas y asistencia como "vacía".
void limpiar_fila(Estado& e, size_t fila) {
    for (int j = 0; j < MAX_CURSOS; ++j)   e.notas[fila][j] = SIN_NOTA;
    for (int d = 0; d < DIAS_SEMANA; ++d)  e.asistencia[fila][d] = SIN_REGISTRO;
}
}  // namespace

// Al eliminar al estudiante de la posición `indice`, todas las filas de abajo
// "suben" una posición en AMBAS matrices. Así cada fila sigue perteneciendo al
// estudiante correcto (si no, las notas quedarían desalineadas).
void quitar_estudiante_en(Estado& e, size_t indice) {
    const size_t total = e.estudiantes.size();
    if (indice >= total) return;

    const size_t filas = std::min(total, static_cast<size_t>(MAX_ESTUDIANTES));
    for (size_t fila = indice; fila + 1 < filas; ++fila) {
        for (int j = 0; j < MAX_CURSOS; ++j)   e.notas[fila][j] = e.notas[fila + 1][j];
        for (int d = 0; d < DIAS_SEMANA; ++d)  e.asistencia[fila][d] = e.asistencia[fila + 1][d];
    }
    if (filas > 0) limpiar_fila(e, filas - 1);   // la última fila queda libre

    e.estudiantes.erase(e.estudiantes.begin() + static_cast<std::ptrdiff_t>(indice));
}

// Búsqueda lineal: recorre el vector hasta encontrar el id.
int indice_estudiante_por_id(const Estado& e, int id) {
    for (size_t i = 0; i < e.estudiantes.size(); ++i) {
        if (e.estudiantes[i].id == id) return static_cast<int>(i);
    }
    return -1;
}

int indice_curso_por_id(const Estado& e, int id) {
    for (size_t i = 0; i < e.cursos.size(); ++i) {
        if (e.cursos[i].id == id) return static_cast<int>(i);
    }
    return -1;
}
