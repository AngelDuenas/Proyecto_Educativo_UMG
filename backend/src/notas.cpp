// DUEÑO: P2 · Tareas: T11, T15, T18
// Módulo Notas: matriz notas[estudiante][curso] y estadísticas.
//
//   GET /api/notas          -> matriz completa (null donde no hay nota)
//   PUT /api/notas          {estudiante_id, curso_id, nota}   (nota 0-100, o null para borrarla)
//   GET /api/estadisticas   -> promedios, mayor/menor, aprobados/reprobados
#include "notas.hpp"

#include <algorithm>
#include <mutex>
#include <string>

#include "respuestas.hpp"

namespace {

bool hay_nota(float valor) { return valor >= 0.0f; }   // SIN_NOTA es -1, toda nota válida es >= 0

size_t total_estudiantes(const Estado& e) {
    return std::min(e.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));
}
size_t total_cursos(const Estado& e) {
    return std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS));
}

json nota_a_json(float valor) {
    if (!hay_nota(valor)) return json(nullptr);        // SIN_NOTA se envía como null
    return json(redondear(static_cast<double>(valor)));
}

// Arma {id, nombre, promedio} (promedio = null si no hay notas).
json resumen(int id, const std::string& nombre, bool hay_promedio, double promedio) {
    json j = json::object();
    j["id"] = id;
    j["nombre"] = nombre;
    if (hay_promedio) j["promedio"] = redondear(promedio);
    else              j["promedio"] = nullptr;
    return j;
}

}  // namespace

// Recorre la FILA del estudiante sumando solo las notas que existen.
bool promedio_estudiante(const Estado& e, size_t fila, double& promedio) {
    if (fila >= total_estudiantes(e)) return false;
    double suma = 0.0;
    int cantidad = 0;
    for (size_t j = 0; j < total_cursos(e); ++j) {
        const float nota = e.notas[fila][j];
        if (hay_nota(nota)) {          // se ignora SIN_NOTA para no arruinar el promedio
            suma += nota;
            ++cantidad;
        }
    }
    if (cantidad == 0) return false;
    promedio = suma / cantidad;
    return true;
}

// Recorre la COLUMNA del curso sumando solo las notas que existen.
bool promedio_curso(const Estado& e, size_t columna, double& promedio) {
    if (columna >= total_cursos(e)) return false;
    double suma = 0.0;
    int cantidad = 0;
    for (size_t i = 0; i < total_estudiantes(e); ++i) {
        const float nota = e.notas[i][columna];
        if (hay_nota(nota)) {
            suma += nota;
            ++cantidad;
        }
    }
    if (cantidad == 0) return false;
    promedio = suma / cantidad;
    return true;
}

void registrar_rutas_notas(httplib::Server& svr, Estado& estado) {
    // ---- Matriz completa ----
    svr.Get("/api/notas", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t ne = total_estudiantes(estado);
        const size_t nc = total_cursos(estado);

        json nombres_est = json::array(), ids_est = json::array();
        json nombres_cur = json::array(), ids_cur = json::array();
        for (size_t i = 0; i < ne; ++i) {
            nombres_est.push_back(estado.estudiantes[i].nombre);
            ids_est.push_back(estado.estudiantes[i].id);
        }
        for (size_t j = 0; j < nc; ++j) {
            nombres_cur.push_back(estado.cursos[j].nombre);
            ids_cur.push_back(estado.cursos[j].id);
        }

        json matriz = json::array();
        for (size_t i = 0; i < ne; ++i) {
            json fila = json::array();
            for (size_t j = 0; j < nc; ++j) fila.push_back(nota_a_json(estado.notas[i][j]));
            matriz.push_back(fila);
        }

        json datos = json::object();
        datos["estudiantes"] = nombres_est;
        datos["estudiante_ids"] = ids_est;
        datos["cursos"] = nombres_cur;
        datos["curso_ids"] = ids_cur;
        datos["matriz"] = matriz;
        responder_ok(res, datos);
    });

    // ---- Ingresar / cambiar una nota ----
    svr.Put("/api/notas", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;

        if (!cuerpo.contains("estudiante_id") || !cuerpo.at("estudiante_id").is_number_integer()) {
            responder_error(res, "Falta 'estudiante_id' (número entero).", 400);
            return;
        }
        if (!cuerpo.contains("curso_id") || !cuerpo.at("curso_id").is_number_integer()) {
            responder_error(res, "Falta 'curso_id' (número entero).", 400);
            return;
        }
        if (!cuerpo.contains("nota")) {
            responder_error(res, "Falta 'nota' (de 0 a 100, o null para borrarla).", 400);
            return;
        }

        float nota = SIN_NOTA;
        const json& jn = cuerpo.at("nota");
        if (jn.is_null()) {
            nota = SIN_NOTA;
        } else if (jn.is_number()) {
            const double valor = jn.get<double>();
            if (valor < 0.0 || valor > 100.0) {
                responder_error(res, "La nota debe estar entre 0 y 100.", 400);
                return;
            }
            nota = static_cast<float>(valor);
        } else {
            responder_error(res, "La nota debe ser un número entre 0 y 100.", 400);
            return;
        }

        std::lock_guard<std::mutex> candado(estado.mtx);
        const int fila = indice_estudiante_por_id(estado, static_cast<int>(cuerpo.at("estudiante_id").get<long long>()));
        if (fila < 0) {
            responder_error(res, "No existe un estudiante con ese id.", 404);
            return;
        }
        const int columna = indice_curso_por_id(estado, static_cast<int>(cuerpo.at("curso_id").get<long long>()));
        if (columna < 0) {
            responder_error(res, "No existe un curso con ese id.", 404);
            return;
        }
        if (fila >= MAX_ESTUDIANTES || columna >= MAX_CURSOS) {
            responder_error(res, "La posición queda fuera de la matriz de notas.", 500);
            return;
        }

        estado.notas[fila][columna] = nota;

        json datos = json::object();
        datos["estudiante_id"] = estado.estudiantes[static_cast<size_t>(fila)].id;
        datos["curso_id"] = estado.cursos[static_cast<size_t>(columna)].id;
        datos["nota"] = nota_a_json(nota);
        responder_ok(res, datos);
    });

    // ---- Estadísticas ----
    svr.Get("/api/estadisticas", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t ne = total_estudiantes(estado);
        const size_t nc = total_cursos(estado);

        // Promedio de cada estudiante (por fila) + mayor, menor, aprobados y reprobados.
        json por_estudiante = json::array();
        int aprobados = 0, reprobados = 0, sin_notas = 0;
        bool hay_extremos = false;
        size_t i_mayor = 0, i_menor = 0;
        double p_mayor = 0.0, p_menor = 0.0;

        for (size_t i = 0; i < ne; ++i) {
            double promedio = 0.0;
            const bool tiene = promedio_estudiante(estado, i, promedio);
            por_estudiante.push_back(resumen(estado.estudiantes[i].id, estado.estudiantes[i].nombre, tiene, promedio));
            if (!tiene) {
                ++sin_notas;
                continue;
            }
            if (promedio >= NOTA_APROBACION) ++aprobados;
            else                             ++reprobados;

            if (!hay_extremos) {
                hay_extremos = true;
                i_mayor = i_menor = i;
                p_mayor = p_menor = promedio;
            } else {
                if (promedio > p_mayor) { p_mayor = promedio; i_mayor = i; }
                if (promedio < p_menor) { p_menor = promedio; i_menor = i; }
            }
        }

        // Promedio de cada curso (por columna).
        json por_curso = json::array();
        for (size_t j = 0; j < nc; ++j) {
            double promedio = 0.0;
            const bool tiene = promedio_curso(estado, j, promedio);
            por_curso.push_back(resumen(estado.cursos[j].id, estado.cursos[j].nombre, tiene, promedio));
        }

        // Promedio general: de TODAS las notas que existen en la matriz.
        double suma = 0.0;
        int cantidad = 0;
        for (size_t i = 0; i < ne; ++i) {
            for (size_t j = 0; j < nc; ++j) {
                if (hay_nota(estado.notas[i][j])) {
                    suma += estado.notas[i][j];
                    ++cantidad;
                }
            }
        }

        json datos = json::object();
        datos["promedio_por_estudiante"] = por_estudiante;
        datos["promedio_por_curso"] = por_curso;
        if (hay_extremos) {
            datos["mayor_promedio"] = resumen(estado.estudiantes[i_mayor].id, estado.estudiantes[i_mayor].nombre, true, p_mayor);
            datos["menor_promedio"] = resumen(estado.estudiantes[i_menor].id, estado.estudiantes[i_menor].nombre, true, p_menor);
        } else {
            datos["mayor_promedio"] = nullptr;
            datos["menor_promedio"] = nullptr;
        }
        datos["aprobados"] = aprobados;
        datos["reprobados"] = reprobados;
        datos["sin_notas"] = sin_notas;
        if (cantidad > 0) datos["promedio_general"] = redondear(suma / cantidad);
        else              datos["promedio_general"] = nullptr;
        responder_ok(res, datos);
    });
}
