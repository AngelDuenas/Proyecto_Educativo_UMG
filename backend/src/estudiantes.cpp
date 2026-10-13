// DUEÑO: P1 · Tareas: T10
// Módulo Estudiantes: CRUD sobre std::vector<Estudiante>.
//
//   GET    /api/estudiantes?buscar=&orden=nombre|id
//   GET    /api/estudiantes/:id
//   POST   /api/estudiantes        {nombre, edad, grado, seccion}  -> 201
//   PUT    /api/estudiantes/:id    (se pueden enviar solo los campos a cambiar)
//   DELETE /api/estudiantes/:id
#include "estudiantes.hpp"

#include <cctype>
#include <mutex>
#include <string>
#include <vector>

#include "respuestas.hpp"

namespace {

// ---------- utilidades de texto ----------

std::string minusculas(std::string texto) {
    for (char& c : texto) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return texto;
}

// Quita espacios al inicio y al final.
std::string recortar(const std::string& texto) {
    const std::string espacios = " \t\r\n";
    const size_t inicio = texto.find_first_not_of(espacios);
    if (inicio == std::string::npos) return "";
    const size_t fin = texto.find_last_not_of(espacios);
    return texto.substr(inicio, fin - inicio + 1);
}

json estudiante_a_json(const Estudiante& e) {
    json j = json::object();
    j["id"] = e.id;
    j["nombre"] = e.nombre;
    j["edad"] = e.edad;
    j["grado"] = e.grado;
    j["seccion"] = e.seccion;
    return j;
}

// ---------- ordenamiento (insertion sort escrito a mano) ----------

// ¿`a` debe ir DESPUÉS de `b` en la lista ordenada?
bool va_despues(const Estudiante& a, const Estudiante& b, bool por_nombre) {
    if (por_nombre) return minusculas(a.nombre) > minusculas(b.nombre);
    return a.id > b.id;
}

// Ordenamiento por inserción: toma cada elemento y lo "inserta" en su lugar
// dentro de la parte que ya está ordenada.
void ordenar_por_insercion(std::vector<Estudiante>& lista, bool por_nombre) {
    for (size_t i = 1; i < lista.size(); ++i) {
        const Estudiante clave = lista[i];
        size_t j = i;
        while (j > 0 && va_despues(lista[j - 1], clave, por_nombre)) {
            lista[j] = lista[j - 1];   // desplaza hacia la derecha
            --j;
        }
        lista[j] = clave;
    }
}

// ---------- validación de los datos que llegan por JSON ----------
// Cada función devuelve "" si todo está bien, o el mensaje de error.

std::string leer_campo_texto(const json& c, const std::string& campo, bool obligatorio, std::string& destino) {
    if (!c.contains(campo)) {
        if (obligatorio) return "Falta el campo '" + campo + "'.";
        return "";
    }
    if (!c.at(campo).is_string()) return "El campo '" + campo + "' debe ser texto.";
    const std::string valor = recortar(c.at(campo).get<std::string>());
    if (valor.empty()) return "El campo '" + campo + "' no puede estar vacío.";
    if (valor.size() > 100) return "El campo '" + campo + "' es demasiado largo.";
    destino = valor;
    return "";
}

std::string leer_campo_edad(const json& c, bool obligatorio, int& destino) {
    if (!c.contains("edad")) {
        if (obligatorio) return "Falta el campo 'edad'.";
        return "";
    }
    if (!c.at("edad").is_number_integer()) return "La edad debe ser un número entero.";
    const long long edad = c.at("edad").get<long long>();
    if (edad < 3 || edad > 100) return "La edad debe estar entre 3 y 100.";
    destino = static_cast<int>(edad);
    return "";
}

// Valida el JSON y copia los valores en `e`. Si hay error, devuelve el mensaje.
std::string validar_y_aplicar(const json& c, bool obligatorios, Estudiante& e) {
    std::string error = leer_campo_texto(c, "nombre", obligatorios, e.nombre);
    if (!error.empty()) return error;
    error = leer_campo_edad(c, obligatorios, e.edad);
    if (!error.empty()) return error;
    error = leer_campo_texto(c, "grado", obligatorios, e.grado);
    if (!error.empty()) return error;
    return leer_campo_texto(c, "seccion", obligatorios, e.seccion);
}

int id_de_la_ruta(const httplib::Request& req) {
    return std::stoi(req.matches[1].str());
}

}  // namespace

void registrar_rutas_estudiantes(httplib::Server& svr, Estado& estado) {
    // ---- Listar (con búsqueda y orden) ----
    svr.Get("/api/estudiantes", [&estado](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);

        const std::string buscar = minusculas(recortar(req.get_param_value("buscar")));
        const std::string orden = req.has_param("orden") ? req.get_param_value("orden") : "id";
        if (orden != "id" && orden != "nombre") {
            responder_error(res, "El parámetro 'orden' debe ser 'id' o 'nombre'.", 400);
            return;
        }

        // Búsqueda lineal: se queda con los estudiantes cuyo nombre contiene el texto buscado.
        std::vector<Estudiante> lista;
        for (const Estudiante& e : estado.estudiantes) {
            if (buscar.empty() || minusculas(e.nombre).find(buscar) != std::string::npos) {
                lista.push_back(e);
            }
        }
        ordenar_por_insercion(lista, orden == "nombre");

        json arreglo = json::array();
        for (const Estudiante& e : lista) arreglo.push_back(estudiante_a_json(e));
        responder_ok(res, arreglo);
    });

    // ---- Obtener uno ----
    svr.Get(R"(/api/estudiantes/(\d{1,9}))", [&estado](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int indice = indice_estudiante_por_id(estado, id_de_la_ruta(req));
        if (indice < 0) {
            responder_error(res, "No existe un estudiante con ese id.", 404);
            return;
        }
        responder_ok(res, estudiante_a_json(estado.estudiantes[static_cast<size_t>(indice)]));
    });

    // ---- Registrar ----
    svr.Post("/api/estudiantes", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;

        std::lock_guard<std::mutex> candado(estado.mtx);
        if (estado.estudiantes.size() >= static_cast<size_t>(MAX_ESTUDIANTES)) {
            responder_error(res, "Se alcanzó el máximo de estudiantes (" + std::to_string(MAX_ESTUDIANTES) + ").", 400);
            return;
        }

        Estudiante nuevo{};
        const std::string error = validar_y_aplicar(cuerpo, true, nuevo);
        if (!error.empty()) {
            responder_error(res, error, 400);
            return;
        }

        nuevo.id = estado.siguiente_id_estudiante++;
        estado.estudiantes.push_back(nuevo);

        // La fila nueva de las matrices empieza vacía.
        const size_t fila = estado.estudiantes.size() - 1;
        for (int j = 0; j < MAX_CURSOS; ++j)   estado.notas[fila][j] = SIN_NOTA;
        for (int d = 0; d < DIAS_SEMANA; ++d)  estado.asistencia[fila][d] = SIN_REGISTRO;

        responder_ok(res, estudiante_a_json(nuevo), 201);
    });

    // ---- Editar ----
    svr.Put(R"(/api/estudiantes/(\d{1,9}))", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;

        std::lock_guard<std::mutex> candado(estado.mtx);
        const int indice = indice_estudiante_por_id(estado, id_de_la_ruta(req));
        if (indice < 0) {
            responder_error(res, "No existe un estudiante con ese id.", 404);
            return;
        }

        // Se trabaja sobre una copia: si hay un error de validación, el original no se toca.
        Estudiante actualizado = estado.estudiantes[static_cast<size_t>(indice)];
        const std::string error = validar_y_aplicar(cuerpo, false, actualizado);
        if (!error.empty()) {
            responder_error(res, error, 400);
            return;
        }
        estado.estudiantes[static_cast<size_t>(indice)] = actualizado;
        responder_ok(res, estudiante_a_json(actualizado));
    });

    // ---- Eliminar ----
    svr.Delete(R"(/api/estudiantes/(\d{1,9}))", [&estado](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int id = id_de_la_ruta(req);
        const int indice = indice_estudiante_por_id(estado, id);
        if (indice < 0) {
            responder_error(res, "No existe un estudiante con ese id.", 404);
            return;
        }
        quitar_estudiante_en(estado, static_cast<size_t>(indice));   // también desplaza las matrices
        json datos = json::object();
        datos["eliminado"] = id;
        responder_ok(res, datos);
    });
}
