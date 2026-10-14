// Servidor HTTP del sistema (cpp-httplib).
// REGLA: este archivo solo crea el servidor y conecta cada ruta con la función de su módulo.
// Las rutas y los formatos JSON son los mismos que simula el modo demo de frontend/js/api.js.
#include <cstdlib>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>

#include "arbol.hpp"
#include "asistencia.hpp"
#include "docentes.hpp"
#include "estudiantes.hpp"
#include "grafo.hpp"
#include "notas.hpp"
#include "reportes.hpp"
#include "respuestas.hpp"

using json = nlohmann::json;
using httplib::Request;
using httplib::Response;

// Los módulos guardan sus datos en variables globales: un solo candado para todas las rutas.
static std::mutex candadoDatos;

template <typename Funcion>
static httplib::Server::Handler conCandado(Funcion funcion) {
    return [funcion](const Request& req, Response& res) {
        std::lock_guard<std::mutex> candado(candadoDatos);
        funcion(req, res);
    };
}

static int leerPuerto() {
    const char* valor = std::getenv("PUERTO");
    if (valor == nullptr) return 8080;
    int puerto = std::atoi(valor);
    return (puerto > 0 && puerto < 65536) ? puerto : 8080;
}

// Busca el último elemento de una lista JSON que cumpla una condición (para devolver lo recién creado).
template <typename Condicion>
static json buscarEn(const std::string& listaJSON, Condicion cumple) {
    json lista = json::parse(listaJSON, nullptr, false);
    if (!lista.is_array()) return nullptr;
    for (auto it = lista.rbegin(); it != lista.rend(); ++it)
        if (cumple(*it)) return *it;
    return nullptr;
}

// Al borrar un docente o un curso se quitan también sus asignaciones.
static void quitarAsignacionesDe(const std::string& docenteId, const std::string& cursoCodigo) {
    json lista = json::parse(obtenerAsignacionesJSON(), nullptr, false);
    if (!lista.is_array()) return;
    for (const auto& a : lista) {
        const std::string d = a.value("docenteId", "");
        const std::string c = a.value("cursoCodigo", "");
        if ((!docenteId.empty() && d == docenteId) || (!cursoCodigo.empty() && c == cursoCodigo))
            eliminarAsignacion(d, c);
    }
}

static void registrarEstudiantes(httplib::Server& svr) {
    svr.Get("/api/estudiantes", conCandado([](const Request& req, Response& res) {
        responderJSON(res, obtenerEstudiantesJSON(req.get_param_value("buscar"), req.get_param_value("orden")));
    }));

    svr.Post("/api/estudiantes", conCandado([](const Request& req, Response& res) {
        json d;
        if (!leerCuerpo(req, res, d)) return;
        const std::string carnet = campoTexto(d, "carnet");
        const std::string nombre = campoTexto(d, "nombre");
        if (carnet.empty() || nombre.empty()) return responderError(res, "El carnet y el nombre son obligatorios", 400);
        if (!crearEstudiante(carnet, nombre, campoTexto(d, "carrera"), campoEntero(d, "semestre", 1)))
            return responderError(res, "Ya existe un estudiante con el carnet " + carnet + " o la lista está llena", 409);
        responderJSON(res, d, 201);
    }));

    svr.Put("/api/estudiantes/:carnet", conCandado([](const Request& req, Response& res) {
        json d;
        if (!leerCuerpo(req, res, d)) return;
        const std::string carnet = req.path_params.at("carnet");
        if (!actualizarEstudiante(carnet, campoTexto(d, "nombre"), campoTexto(d, "carrera"), campoEntero(d, "semestre", 1)))
            return responderError(res, "No existe el estudiante " + carnet, 404);
        d["carnet"] = carnet;
        responderJSON(res, d);
    }));

    svr.Delete("/api/estudiantes/:carnet", conCandado([](const Request& req, Response& res) {
        const std::string carnet = req.path_params.at("carnet");
        if (!eliminarEstudiante(carnet)) return responderError(res, "No existe el estudiante " + carnet, 404);
        responderJSON(res, json{{"ok", true}});
    }));
}

static void registrarDocentesYCursos(httplib::Server& svr) {
    // ----- Docentes -----
    svr.Get("/api/docentes", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerDocentesJSON());
    }));

    svr.Post("/api/docentes", conCandado([](const Request& req, Response& res) {
        json d;
        if (!leerCuerpo(req, res, d)) return;
        const std::string nombre = campoTexto(d, "nombre");
        if (nombre.empty()) return responderError(res, "El nombre del docente es obligatorio", 400);
        if (!crearDocente(nombre, campoTexto(d, "especialidad")))
            return responderError(res, "Ya no caben más docentes", 409);
        responderJSON(res, buscarEn(obtenerDocentesJSON(), [&](const json& x) { return x.value("nombre", "") == nombre; }), 201);
    }));

    svr.Delete("/api/docentes/:id", conCandado([](const Request& req, Response& res) {
        const std::string id = req.path_params.at("id");
        if (!eliminarDocente(id)) return responderError(res, "No existe el docente " + id, 404);
        quitarAsignacionesDe(id, "");
        responderJSON(res, json{{"ok", true}});
    }));

    // ----- Cursos -----
    svr.Get("/api/cursos", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerCursosJSON());
    }));

    svr.Post("/api/cursos", conCandado([](const Request& req, Response& res) {
        json d;
        if (!leerCuerpo(req, res, d)) return;
        const std::string codigo = campoTexto(d, "codigo");
        const std::string nombre = campoTexto(d, "nombre");
        if (codigo.empty() || nombre.empty()) return responderError(res, "El código y el nombre son obligatorios", 400);
        if (!crearCurso(codigo, nombre, campoEntero(d, "creditos", 0), campoEntero(d, "semestre", 1)))
            return responderError(res, "Ya existe un curso con el código " + codigo + " o la lista está llena", 409);
        responderJSON(res, d, 201);
    }));

    svr.Delete("/api/cursos/:codigo", conCandado([](const Request& req, Response& res) {
        const std::string codigo = req.path_params.at("codigo");
        if (!eliminarCurso(codigo)) return responderError(res, "No existe el curso " + codigo, 404);
        quitarAsignacionesDe("", codigo);
        responderJSON(res, json{{"ok", true}});
    }));

    // ----- Asignaciones -----
    svr.Get("/api/asignaciones", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerAsignacionesJSON());
    }));

    svr.Post("/api/asignaciones", conCandado([](const Request& req, Response& res) {
        json d;
        if (!leerCuerpo(req, res, d)) return;
        const std::string docenteId = campoTexto(d, "docenteId");
        const std::string cursoCodigo = campoTexto(d, "cursoCodigo");
        if (docenteId.empty() || cursoCodigo.empty()) return responderError(res, "Elige un docente y un curso", 400);
        json existente = buscarEn(obtenerAsignacionesJSON(), [&](const json& x) {
            return x.value("docenteId", "") == docenteId && x.value("cursoCodigo", "") == cursoCodigo;
        });
        if (!existente.is_null()) return responderError(res, "Ese docente ya tiene asignado ese curso", 409);
        if (!crearAsignacion(docenteId, cursoCodigo)) return responderError(res, "Ya no caben más asignaciones", 409);
        responderJSON(res, json{{"docenteId", docenteId}, {"cursoCodigo", cursoCodigo}}, 201);
    }));

    svr.Delete("/api/asignaciones/:docente/:curso", conCandado([](const Request& req, Response& res) {
        if (!eliminarAsignacion(req.path_params.at("docente"), req.path_params.at("curso")))
            return responderError(res, "No existe esa asignación", 404);
        responderJSON(res, json{{"ok", true}});
    }));
}

static void registrarNotasYAsistencia(httplib::Server& svr) {
    svr.Get("/api/notas", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerNotasJSON());
    }));

    svr.Put("/api/notas", conCandado([](const Request& req, Response& res) {
        json d;
        if (!leerCuerpo(req, res, d)) return;
        const std::string carnet = campoTexto(d, "carnet");
        const std::string curso = campoTexto(d, "curso");
        if (carnet.empty() || curso.empty()) return responderError(res, "Faltan el carnet o el curso", 400);

        float nota = 0.0f;
        float* puntero = nullptr;  // null = borrar la nota
        if (d.contains("nota") && !d["nota"].is_null()) {
            if (!d["nota"].is_number()) return responderError(res, "La nota debe ser un número", 400);
            nota = d["nota"].get<float>();
            if (nota < 0.0f || nota > 100.0f) return responderError(res, "La nota debe estar entre 0 y 100", 400);
            puntero = &nota;
        }
        if (!actualizarNota(carnet, curso, puntero)) return responderError(res, "No se pudo guardar la nota", 404);
        responderJSON(res, d);
    }));

    svr.Get("/api/estadisticas", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerEstadisticasJSON());
    }));

    svr.Get("/api/asistencia", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerAsistenciaJSON());
    }));

    svr.Put("/api/asistencia", conCandado([](const Request& req, Response& res) {
        json d;
        if (!leerCuerpo(req, res, d)) return;
        const std::string carnet = campoTexto(d, "carnet");
        const int dia = campoEntero(d, "dia", -1);
        const int valor = campoEntero(d, "valor", 1) ? 1 : 0;
        if (carnet.empty()) return responderError(res, "Falta el carnet", 400);
        if (!actualizarAsistencia(carnet, dia, valor)) return responderError(res, "El día debe estar entre 0 (L) y 4 (V)", 400);
        responderJSON(res, d);
    }));

    svr.Get("/api/reportes", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerReportesJSON());
    }));
}

static void registrarArbolYGrafo(httplib::Server& svr) {
    svr.Get("/api/arbol", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerArbolJSON());
    }));

    svr.Get("/api/grafo", conCandado([](const Request&, Response& res) {
        responderJSON(res, obtenerGrafoJSON());
    }));

    // /api/grafo/bfs?origen=MAT101  ·  /api/grafo/dfs?origen=MAT101
    svr.Get("/api/grafo/:tipo", conCandado([](const Request& req, Response& res) {
        const std::string tipo = req.path_params.at("tipo");
        if (tipo != "bfs" && tipo != "dfs") return responderError(res, "Recorrido desconocido: usa bfs o dfs", 404);
        const std::string origen = req.get_param_value("origen");
        std::string salida;
        if (!recorrerGrafoJSON(origen, tipo == "bfs", salida))
            return responderError(res, "No existe el curso " + origen, 404);
        responderJSON(res, salida);
    }));
}

int main() {
    inicializarEstudiantes();
    inicializarDocentesYCursos();
    inicializarNotas();
    inicializarAsistencia();

    httplib::Server svr;

    // CORS permisivo: así también funciona si el frontend se abre con Live Server.
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"},
    });
    svr.Options(R"(.*)", [](const Request&, Response& res) { res.status = 204; });

    // Errores no controlados → 500 con { "error": ... }
    svr.set_exception_handler([](const Request&, Response& res, std::exception_ptr ep) {
        std::string mensaje = "Error interno del servidor";
        try {
            if (ep) std::rethrow_exception(ep);
        } catch (const std::exception& ex) {
            mensaje += std::string(": ") + ex.what();
        } catch (...) {
        }
        responderError(res, mensaje, 500);
    });

    // Rutas /api que no existen → 404 en JSON (las páginas del frontend no se tocan)
    svr.set_error_handler([](const Request& req, Response& res) {
        if (res.body.empty() && req.path.rfind("/api", 0) == 0)
            responderError(res, "La ruta " + req.method + " " + req.path + " no existe", res.status);
    });

    // Frontend como archivos estáticos: http://localhost:8080/
    if (!svr.set_mount_point("/", "./frontend"))
        std::cerr << "Aviso: no se encontró ./frontend (ejecuta el servidor desde la raíz del repo)\n";

    svr.Get("/api/salud", [](const Request&, Response& res) {
        responderJSON(res, json{{"servicio", "gestion-escolar"}, {"version", "0.1.0"}});
    });

    registrarEstudiantes(svr);
    registrarDocentesYCursos(svr);
    registrarNotasYAsistencia(svr);
    registrarArbolYGrafo(svr);

    const int puerto = leerPuerto();
    std::cout << "Servidor listo en http://localhost:" << puerto << std::endl;
    if (!svr.listen("0.0.0.0", puerto)) {
        std::cerr << "No se pudo abrir el puerto " << puerto << " (¿ya hay otro servidor corriendo?)" << std::endl;
        return 1;
    }
    return 0;
}
