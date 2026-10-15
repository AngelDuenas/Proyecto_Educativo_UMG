// DUEÑO: P1 · main.cpp SOLO crea el servidor y registra las rutas de cada módulo
// (así nadie edita el mismo archivo y se evitan conflictos de Git).
#include <cstdlib>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>

#include "httplib.h"
#include "json.hpp"

#include "algebra.hpp"
#include "arbol.hpp"
#include "asistencia.hpp"
#include "avisos.hpp"
#include "docentes.hpp"          // módulo de Cristian
#include "estado.hpp"
#include "estudiantes.hpp"
#include "grafo.hpp"
#include "horario.hpp"
#include "inscripciones.hpp"
#include "memoria.hpp"
#include "notas.hpp"
#include "persistencia.hpp"
#include "reportes.hpp"
#include "respuestas.hpp"
#include "rutas_catalogo.hpp"

int main() {
    Estado& estado = estado_global();
    inicializar_matrices(estado);
    cargar_datos_ejemplo(estado);      // arranca con los datos de ejemplo...
    cargar_ejemplo_inscripciones(estado);
    cargar_ejemplo_horario(estado);
    cargar_ejemplo_avisos(estado);
    inicializarDocentesYCursos();      // (docentes de ejemplo del módulo de Cristian)
    cargar_todo(estado);               // ...y si data/ tiene datos válidos, esos los reemplazan

    httplib::Server svr;

    // CORS permisivo (útil si el frontend se abre desde otro origen).
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });
    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // Cualquier error inesperado en un handler se responde con el formato estándar.
    svr.set_exception_handler([](const httplib::Request&, httplib::Response& res, std::exception_ptr ep) {
        std::string detalle = "Error interno del servidor.";
        try {
            if (ep) std::rethrow_exception(ep);
        } catch (const std::exception& ex) {
            detalle += std::string(" ") + ex.what();
        } catch (...) {
        }
        responder_error(res, detalle, 500);
    });

    // El backend también sirve el frontend (ejecutar desde la raíz del repo).
    svr.set_mount_point("/", "./frontend");

    svr.Get("/api/salud", [](const httplib::Request&, httplib::Response& res) {
        json datos = json::object();
        datos["servicio"] = "gestion-escolar";
        datos["version"] = "0.2.0";
        responder_json(res, datos);
    });

    registrar_rutas_estudiantes(svr, estado);
    registrar_rutas_notas(svr, estado);
    registrar_rutas_asistencia(svr, estado);
    registrar_rutas_reportes(svr, estado);
    registrar_rutas_grafo(svr, estado);
    registrar_rutas_arbol(svr, estado);        // de muestra (solo lectura)
    registrar_rutas_algebra(svr, estado);      // de muestra (solo lectura)
    registrar_rutas_memoria(svr, estado);      // matrices crudas en RAM (solo lectura)
    registrar_rutas_inscripciones(svr, estado);
    registrar_rutas_horario(svr, estado);
    registrar_rutas_avisos(svr, estado);
    registrar_rutas_catalogo(svr, estado);
    registrar_rutas_persistencia(svr, estado);

    // Guardado automático: después de cada cambio que salió bien (POST/PUT/DELETE con código < 400)
    // se escriben los archivos de data/, así nada se pierde al cerrar el servidor.
    svr.set_post_routing_handler([&estado](const httplib::Request& req, httplib::Response& res) {
        const bool cambia_datos = req.method == "POST" || req.method == "PUT" || req.method == "DELETE";
        const bool es_api = req.path.rfind("/api/", 0) == 0 && req.path != "/api/guardar" && req.path != "/api/cargar";
        if (!cambia_datos || !es_api || res.status >= 400) return;
        std::lock_guard<std::mutex> candado(estado.mtx);
        if (!guardar_todo(estado)) std::cerr << "[persistencia] No se pudo guardar en data/ tras " << req.method << " " << req.path << std::endl;
    });

    int puerto = 8080;
    if (const char* texto = std::getenv("PUERTO")) {
        try {
            puerto = std::stoi(texto);
        } catch (...) {
            std::cerr << "PUERTO invalido, se usa 8080" << std::endl;
        }
    }

    // Por defecto solo responde en esta PC. Con HOST=0.0.0.0 acepta conexiones de la red.
    const char* host_env = std::getenv("HOST");
    const std::string host = (host_env != nullptr && !std::string(host_env).empty()) ? host_env : "127.0.0.1";

    if (!svr.bind_to_port(host, puerto)) {
        std::cerr << "No se pudo abrir " << host << ":" << puerto << " (ya hay otro servidor corriendo?)" << std::endl;
        return 1;
    }
    // 127.0.0.1 y no localhost: en Windows localhost intenta primero IPv6 (::1) y cada conexion tarda ~2 s.
    std::cout << "Servidor listo en http://127.0.0.1:" << puerto << std::endl;
    if (host != "127.0.0.1") std::cout << "Escuchando en " << host << ":" << puerto << " (accesible desde la red)" << std::endl;
    svr.listen_after_bind();
    return 0;
}
