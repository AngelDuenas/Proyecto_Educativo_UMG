// DUEÑO: P1 · Módulo Estudiantes (CRUD) — formato definido por la vista-estudiantes.js
//   GET    /api/estudiantes?buscar=&orden=nombre|nombre-desc|carnet|semestre
//   POST   /api/estudiantes          {carnet, nombre, carrera, semestre}
//   PUT    /api/estudiantes/{carnet} {nombre, carrera, semestre}   (se puede mandar solo lo que cambia)
//   DELETE /api/estudiantes/{carnet}
#include "estudiantes.hpp"

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include "respuestas.hpp"

namespace {

std::string recortar(const std::string& texto) {
    const std::string espacios = " \t\r\n";
    const size_t inicio = texto.find_first_not_of(espacios);
    if (inicio == std::string::npos) return "";
    const size_t fin = texto.find_last_not_of(espacios);
    return texto.substr(inicio, fin - inicio + 1);
}

json estudiante_a_json(const Estudiante& e) {
    json j = json::object();
    j["carnet"] = e.carnet;
    j["nombre"] = e.nombre;
    j["carrera"] = e.carrera;
    j["semestre"] = e.semestre;
    return j;
}

// Lee un campo de texto opcional. Devuelve false si existe pero no es texto.
bool leer_texto(const json& c, const char* campo, std::string& destino, bool& presente) {
    presente = c.contains(campo) && !c.at(campo).is_null();
    if (!presente) return true;
    if (!c.at(campo).is_string()) return false;
    destino = recortar(c.at(campo).get<std::string>());
    return true;
}

// Acepta 2 o "2". Devuelve false si no es un entero entre 1 y 12.
bool leer_semestre(const json& c, int& destino, bool& presente) {
    presente = c.contains("semestre") && !c.at("semestre").is_null();
    if (!presente) return true;
    const json& v = c.at("semestre");
    long long n = 0;
    if (v.is_number_integer()) {
        n = v.get<long long>();
    } else if (v.is_string()) {
        const std::string texto = v.get<std::string>();
        try {
            size_t usados = 0;
            n = std::stoll(texto, &usados);
            if (usados != texto.size()) return false;
        } catch (...) {
            return false;
        }
    } else {
        return false;
    }
    if (n < 1 || n > 12) return false;
    destino = static_cast<int>(n);
    return true;
}

// Aplica los campos del JSON sobre `e`. Devuelve "" si todo está bien, o el mensaje de error.
std::string aplicar_campos(const json& c, Estudiante& e) {
    bool presente = false;
    if (!leer_texto(c, "nombre", e.nombre, presente)) return "El nombre debe ser texto.";
    if (presente && e.nombre.empty()) return "El carnet y el nombre son obligatorios.";
    if (e.nombre.size() > 100) return "El nombre es demasiado largo.";
    if (!leer_texto(c, "carrera", e.carrera, presente)) return "La carrera debe ser texto.";
    if (e.carrera.size() > 100) return "La carrera es demasiado larga.";
    if (!leer_semestre(c, e.semestre, presente)) return "El semestre debe ser un número entero entre 1 y 12.";
    return "";
}

}  // namespace

void registrar_rutas_estudiantes(httplib::Server& svr, Estado& estado) {
    // ---- Listar, con búsqueda y orden ----
    svr.Get("/api/estudiantes", [&estado](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);

        const std::string buscar = recortar(req.get_param_value("buscar"));
        const std::string buscar_norm = normalizar_texto(buscar);
        const std::string orden = req.has_param("orden") ? req.get_param_value("orden") : "nombre";

        // Búsqueda lineal: se queda con quien coincide por nombre (sin tildes) o por carnet.
        std::vector<Estudiante> lista;
        for (const Estudiante& e : estado.estudiantes) {
            if (buscar.empty() || normalizar_texto(e.nombre).find(buscar_norm) != std::string::npos ||
                e.carnet.find(buscar) != std::string::npos) {
                lista.push_back(e);
            }
        }
        ordenar_estudiantes(lista, orden);

        json arreglo = json::array();
        for (const Estudiante& e : lista) arreglo.push_back(estudiante_a_json(e));
        responder_json(res, arreglo);
    });

    // ---- Registrar ----
    svr.Post("/api/estudiantes", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;

        Estudiante nuevo;
        bool presente = false;
        if (!leer_texto(cuerpo, "carnet", nuevo.carnet, presente) || nuevo.carnet.empty() ||
            !leer_texto(cuerpo, "nombre", nuevo.nombre, presente) || nuevo.nombre.empty()) {
            responder_error(res, "El carnet y el nombre son obligatorios.", 400);
            return;
        }
        if (nuevo.carnet.size() > 30) {
            responder_error(res, "El carnet es demasiado largo.", 400);
            return;
        }
        const std::string error = aplicar_campos(cuerpo, nuevo);
        if (!error.empty()) {
            responder_error(res, error, 400);
            return;
        }

        std::lock_guard<std::mutex> candado(estado.mtx);
        const Resultado r = agregar_estudiante(estado, nuevo);
        if (r == Resultado::DUPLICADO) {
            responder_error(res, "Ya existe un estudiante con el carnet " + nuevo.carnet + ".", 409);
            return;
        }
        if (r == Resultado::LLENO) {
            responder_error(res, "Se alcanzó el máximo de estudiantes (" + std::to_string(MAX_ESTUDIANTES) + ").", 400);
            return;
        }
        std::cout << "[matriz] Estudiante " << nuevo.carnet << " -> fila " << (estado.estudiantes.size() - 1)
                  << " de notas[" << MAX_ESTUDIANTES << "][" << MAX_CURSOS << "] y asistencia["
                  << MAX_ESTUDIANTES << "][" << DIAS_SEMANA << "]" << std::endl;
        responder_json(res, estudiante_a_json(nuevo), 201);
    });

    // ---- Editar ----
    svr.Put(R"(/api/estudiantes/([^/]+))", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        const std::string carnet = req.matches[1].str();

        std::lock_guard<std::mutex> candado(estado.mtx);
        const int pos = indice_estudiante(estado, carnet);
        if (pos < 0) {
            responder_error(res, "No existe el estudiante " + carnet + ".", 404);
            return;
        }
        // Se trabaja sobre una copia: si hay error de validación, el original no se toca.
        Estudiante actualizado = estado.estudiantes[static_cast<size_t>(pos)];
        const std::string error = aplicar_campos(cuerpo, actualizado);
        if (!error.empty()) {
            responder_error(res, error, 400);
            return;
        }
        estado.estudiantes[static_cast<size_t>(pos)] = actualizado;   // el carnet no cambia
        responder_json(res, estudiante_a_json(actualizado));
    });

    // ---- Eliminar ----
    svr.Delete(R"(/api/estudiantes/([^/]+))", [&estado](const httplib::Request& req, httplib::Response& res) {
        const std::string carnet = req.matches[1].str();
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int fila = indice_estudiante(estado, carnet);
        if (quitar_estudiante(estado, carnet) == Resultado::NO_EXISTE) {   // también desplaza las matrices
            responder_error(res, "No existe el estudiante " + carnet + ".", 404);
            return;
        }
        std::cout << "[matriz] Estudiante " << carnet << " eliminado: se quitó la fila " << fila
                  << " y las de abajo subieron una posición" << std::endl;
        json ok = json::object();
        ok["ok"] = true;
        responder_json(res, ok);
    });
}
