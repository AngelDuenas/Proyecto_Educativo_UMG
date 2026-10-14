// DUEÑO: P1 · main.cpp SOLO crea el servidor y registra las rutas de cada módulo
// (así nadie edita el mismo archivo y se evitan conflictos de Git).
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "httplib.h"
#include "json.hpp"

#include "asistencia.hpp"
#include "docentes.hpp"          // módulo de Cristian
#include "estado.hpp"
#include "estudiantes.hpp"
#include "grafo.hpp"
#include "notas.hpp"
#include "persistencia.hpp"
#include "reportes.hpp"
#include "respuestas.hpp"
#include "rutas_catalogo.hpp"

int main() {
    Estado& estado = estado_global();
    inicializar_matrices(estado);
    cargar_datos_ejemplo(estado);      // arranca con los datos de ejemplo...
    cargar_todo(estado);               // ...y si data/ tiene datos válidos, esos los reemplazan
    inicializarDocentesYCursos();      // datos iniciales del módulo de Cristian

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
    registrar_rutas_catalogo(svr, estado);
    registrar_rutas_persistencia(svr, estado);

    int puerto = 8080;
    if (const char* texto = std::getenv("PUERTO")) {
        try {
            puerto = std::stoi(texto);
        } catch (...) {
            std::cerr << "PUERTO invalido, se usa 8080" << std::endl;
        }
    }

    if (!svr.bind_to_port("127.0.0.1", puerto)) {
        std::cerr << "No se pudo abrir el puerto " << puerto << " (ya hay otro servidor corriendo?)" << std::endl;
        return 1;
    }
    std::cout << "Servidor listo en http://localhost:" << puerto << std::endl;
    svr.listen_after_bind();
    return 0;
}
