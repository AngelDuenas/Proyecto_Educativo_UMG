// DUEÑO: P2 · Módulo Notas — formato definido por vista-calificaciones.js
//   GET /api/notas          -> {estudiantes:[{carnet,nombre}], cursos:[{codigo,nombre}], matriz:[[85,null,...]]}
//   PUT /api/notas          {carnet, curso, nota}   (nota 0-100, o null para borrarla)
//   GET /api/estadisticas   -> promedios, mayor/menor, aprobados/reprobados
#include "notas.hpp"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>

#include "algoritmos.hpp"
#include "respuestas.hpp"

namespace {

size_t filas(const Estado& e)    { return std::min(e.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES)); }
size_t columnas(const Estado& e) { return std::min(e.cursos.size(), static_cast<size_t>(MAX_CURSOS)); }

json nota_a_json(float valor) {
    if (valor < 0.0f) return json(nullptr);              // SIN_NOTA se envía como null
    return json(redondear(static_cast<double>(valor)));
}

json promedio_a_json(bool hay, double promedio) {
    if (!hay) return json(nullptr);
    return json(redondear(promedio));
}

json extremo_a_json(const Extremo& x) {
    if (!x.existe) return json(nullptr);
    json j = json::object();
    j["nota"] = redondear(x.nota);
    j["carnet"] = x.carnet;
    j["curso"] = x.curso;
    return j;
}

}  // namespace

void registrar_rutas_notas(httplib::Server& svr, Estado& estado) {
    // ---- Matriz completa ----
    svr.Get("/api/notas", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t ne = filas(estado), nc = columnas(estado);

        json estudiantes = json::array(), cursos = json::array(), matriz = json::array();
        for (size_t i = 0; i < ne; ++i) {
            json f = json::object();
            f["carnet"] = estado.estudiantes[i].carnet;
            f["nombre"] = estado.estudiantes[i].nombre;
            estudiantes.push_back(f);
        }
        for (size_t j = 0; j < nc; ++j) {
            json c = json::object();
            c["codigo"] = estado.cursos[j].codigo;
            c["nombre"] = estado.cursos[j].nombre;
            cursos.push_back(c);
        }
        for (size_t i = 0; i < ne; ++i) {
            json fila = json::array();
            for (size_t j = 0; j < nc; ++j) fila.push_back(nota_a_json(estado.notas[i][j]));
            matriz.push_back(fila);
        }

        json datos = json::object();
        datos["estudiantes"] = estudiantes;
        datos["cursos"] = cursos;
        datos["matriz"] = matriz;
        responder_json(res, datos);
    });

    // ---- Ingresar, cambiar o borrar una nota ----
    svr.Put("/api/notas", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;

        if (!cuerpo.contains("carnet") || !cuerpo.at("carnet").is_string() ||
            !cuerpo.contains("curso") || !cuerpo.at("curso").is_string()) {
            responder_error(res, "Faltan el carnet y el código del curso.", 400);
            return;
        }
        if (!cuerpo.contains("nota")) {
            responder_error(res, "Falta la nota (de 0 a 100, o null para borrarla).", 400);
            return;
        }

        float nota = SIN_NOTA;
        const json& jn = cuerpo.at("nota");
        if (jn.is_number()) {
            const double valor = jn.get<double>();
            if (valor < 0.0 || valor > 100.0) {
                responder_error(res, "La nota debe estar entre 0 y 100.", 400);
                return;
            }
            nota = static_cast<float>(valor);
        } else if (!jn.is_null()) {
            responder_error(res, "La nota debe estar entre 0 y 100.", 400);
            return;
        }

        const std::string carnet = cuerpo.at("carnet").get<std::string>();
        const std::string curso = cuerpo.at("curso").get<std::string>();

        std::lock_guard<std::mutex> candado(estado.mtx);
        const int fila = indice_estudiante(estado, carnet);
        if (fila < 0) {
            responder_error(res, "No existe el estudiante " + carnet + ".", 404);
            return;
        }
        const int col = indice_curso(estado, curso);
        if (col < 0) {
            responder_error(res, "No existe el curso " + curso + ".", 404);
            return;
        }
        estado.notas[fila][col] = nota;
        std::cout << "[matriz] notas[" << fila << "][" << col << "] = " << nota
                  << "   (" << carnet << ", " << curso << (nota < 0.0f ? ", sin nota" : "") << ")" << std::endl;

        json datos = json::object();
        datos["carnet"] = carnet;
        datos["curso"] = curso;
        datos["nota"] = nota_a_json(nota);
        responder_json(res, datos);
    });

    // ---- Estadísticas ----
    svr.Get("/api/estadisticas", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t ne = filas(estado), nc = columnas(estado);

        json por_estudiante = json::array();
        for (size_t i = 0; i < ne; ++i) {
            double p = 0.0;
            const bool hay = promedio_estudiante(estado, i, p);        // promedio de la FILA
            json o = json::object();
            o["carnet"] = estado.estudiantes[i].carnet;
            o["promedio"] = promedio_a_json(hay, p);
            por_estudiante.push_back(o);
        }
        json por_curso = json::array();
        for (size_t j = 0; j < nc; ++j) {
            double p = 0.0;
            const bool hay = promedio_curso(estado, j, p);             // promedio de la COLUMNA
            json o = json::object();
            o["codigo"] = estado.cursos[j].codigo;
            o["promedio"] = promedio_a_json(hay, p);
            por_curso.push_back(o);
        }

        double general = 0.0;
        const bool hay_general = promedio_general(estado, general);
        Extremo mayor, menor;
        extremos_notas(estado, mayor, menor);
        int aprobados = 0, reprobados = 0;
        contar_aprobados(estado, aprobados, reprobados);

        json datos = json::object();
        datos["promedioGeneral"] = promedio_a_json(hay_general, general);
        datos["porEstudiante"] = por_estudiante;
        datos["porCurso"] = por_curso;
        datos["mayor"] = extremo_a_json(mayor);
        datos["menor"] = extremo_a_json(menor);
        datos["aprobados"] = aprobados;
        datos["reprobados"] = reprobados;
        responder_json(res, datos);
    });
}
