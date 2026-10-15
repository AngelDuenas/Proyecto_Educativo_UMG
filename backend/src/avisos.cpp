// Módulo Avisos / calendario. Formato definido por vista-avisos.js.
#include "avisos.hpp"

#include <algorithm>
#include <cctype>
#include <mutex>
#include <string>

#include "respuestas.hpp"

namespace {

const char* TIPOS[] = {"examen", "entrega", "evento", "aviso"};

bool tipo_valido(const std::string& tipo) {
    for (const char* t : TIPOS)
        if (tipo == t) return true;
    return false;
}

// "AAAA-MM-DD" con un día que exista en ese mes (también revisa años bisiestos).
bool fecha_valida(const std::string& f) {
    if (f.size() != 10 || f[4] != '-' || f[7] != '-') return false;
    for (size_t i = 0; i < f.size(); ++i)
        if (i != 4 && i != 7 && !std::isdigit(static_cast<unsigned char>(f[i]))) return false;
    const int anio = std::stoi(f.substr(0, 4));
    const int mes = std::stoi(f.substr(5, 2));
    const int dia = std::stoi(f.substr(8, 2));
    if (anio < 2000 || anio > 2100 || mes < 1 || mes > 12 || dia < 1) return false;
    const bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0;
    const int dias_mes[] = {31, bisiesto ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return dia <= dias_mes[mes - 1];
}

std::string recortar(const std::string& v) {
    const size_t ini = v.find_first_not_of(" \t\r\n");
    if (ini == std::string::npos) return "";
    return v.substr(ini, v.find_last_not_of(" \t\r\n") - ini + 1);
}

std::string texto(const json& c, const char* campo) {
    if (!c.contains(campo) || !c.at(campo).is_string()) return "";
    return recortar(c.at(campo).get<std::string>());
}

// Valida el cuerpo y llena `a` (sin tocar su id). Si algo falla responde el error y devuelve false.
bool leer_aviso(const json& c, const Estado& e, Aviso& a, httplib::Response& res) {
    a.titulo = texto(c, "titulo");
    a.descripcion = texto(c, "descripcion");
    a.fecha = texto(c, "fecha");
    a.tipo = texto(c, "tipo");
    a.curso = texto(c, "curso");
    if (a.titulo.empty()) {
        responder_error(res, "El título del aviso es obligatorio.", 400);
        return false;
    }
    if (a.titulo.size() > 120) {
        responder_error(res, "El título no puede pasar de 120 caracteres.", 400);
        return false;
    }
    if (!fecha_valida(a.fecha)) {
        responder_error(res, "La fecha debe tener el formato AAAA-MM-DD y ser una fecha real.", 400);
        return false;
    }
    if (a.tipo.empty()) a.tipo = "aviso";
    if (!tipo_valido(a.tipo)) {
        responder_error(res, "El tipo debe ser examen, entrega, evento o aviso.", 400);
        return false;
    }
    if (!a.curso.empty() && indice_curso(e, a.curso) < 0) {
        responder_error(res, "No existe el curso " + a.curso + ".", 404);
        return false;
    }
    return true;
}

json aviso_a_json(const Estado& e, const Aviso& a) {
    json j = json::object();
    j["id"] = a.id;
    j["titulo"] = a.titulo;
    j["descripcion"] = a.descripcion;
    j["fecha"] = a.fecha;
    j["tipo"] = a.tipo;
    j["curso"] = a.curso;
    const int c = a.curso.empty() ? -1 : indice_curso(e, a.curso);
    j["cursoNombre"] = c >= 0 ? json(e.cursos[static_cast<size_t>(c)].nombre) : json(nullptr);
    return j;
}

// Id de la URL (solo dígitos). Uno demasiado largo no puede existir: se trata como -1.
int id_de_url(const std::string& texto) {
    return texto.size() > 9 ? -1 : std::stoi(texto);
}

int buscar(const Estado& e, int id) {
    for (size_t i = 0; i < e.avisos.size(); ++i)
        if (e.avisos[i].id == id) return static_cast<int>(i);
    return -1;
}

}  // namespace

void cargar_ejemplo_avisos(Estado& e) {
    e.avisos = {
        {1, "Inicio del segundo semestre", "Bienvenida en el auditorio principal.", "2026-07-13", "evento", ""},
        {2, "Primer parcial de Matemática I", "Temas 1 a 4. Traer calculadora.", "2026-10-20", "examen", "MAT101"},
        {3, "Entrega del proyecto de Programación I", "Subir el repositorio con el informe en PDF.", "2026-10-23", "entrega", "PRG102"},
        {4, "Feria de Ingeniería", "Presentación de proyectos de todos los semestres.", "2026-10-30", "evento", ""},
        {5, "Examen final de Álgebra Lineal", "Incluye matrices, determinantes y sistemas de ecuaciones.", "2026-11-06", "examen", "ALG201"},
        {6, "Cierre de notas del semestre", "Último día para revisar notas con los docentes.", "2026-11-13", "aviso", ""},
    };
    e.siguiente_id_aviso = 7;
}

json avisos_a_json(const Estado& e) {
    json lista = json::array();
    for (const Aviso& a : e.avisos) {
        json j = aviso_a_json(e, a);
        j.erase("cursoNombre");   // se calcula al consultar
        lista.push_back(j);
    }
    return lista;
}

bool avisos_desde_json(const json& j, Estado& e) {
    if (!j.is_array()) return false;
    std::vector<Aviso> leidos;
    int mayor = 0;
    for (const json& x : j) {
        if (!x.is_object() || !x.contains("id") || !x.at("id").is_number_integer()) return false;
        Aviso a;
        a.id = static_cast<int>(x.at("id").get<long long>());
        a.titulo = texto(x, "titulo");
        a.descripcion = texto(x, "descripcion");
        a.fecha = texto(x, "fecha");
        a.tipo = texto(x, "tipo");
        a.curso = texto(x, "curso");
        if (a.titulo.empty() || !fecha_valida(a.fecha) || !tipo_valido(a.tipo)) return false;
        if (!a.curso.empty() && indice_curso(e, a.curso) < 0) a.curso.clear();
        mayor = std::max(mayor, a.id);
        leidos.push_back(a);
    }
    e.avisos = leidos;
    e.siguiente_id_aviso = mayor + 1;
    return true;
}

void registrar_rutas_avisos(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/avisos", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        std::vector<Aviso> ordenados = estado.avisos;
        // "AAAA-MM-DD" se ordena bien como texto.
        std::stable_sort(ordenados.begin(), ordenados.end(), [](const Aviso& a, const Aviso& b) { return a.fecha < b.fecha; });
        json lista = json::array();
        for (const Aviso& a : ordenados) lista.push_back(aviso_a_json(estado, a));
        responder_json(res, lista);
    });

    svr.Post("/api/avisos", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        std::lock_guard<std::mutex> candado(estado.mtx);
        Aviso nuevo;
        if (!leer_aviso(cuerpo, estado, nuevo, res)) return;
        nuevo.id = estado.siguiente_id_aviso++;
        estado.avisos.push_back(nuevo);
        responder_json(res, aviso_a_json(estado, nuevo), 201);
    });

    svr.Put(R"(/api/avisos/(\d+))", [&estado](const httplib::Request& req, httplib::Response& res) {
        json cuerpo;
        if (!leer_cuerpo_json(req, res, cuerpo)) return;
        const int id = id_de_url(req.matches[1].str());
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int pos = buscar(estado, id);
        if (pos < 0) {
            responder_error(res, "No existe el aviso " + req.matches[1].str() + ".", 404);
            return;
        }
        Aviso editado;
        if (!leer_aviso(cuerpo, estado, editado, res)) return;
        editado.id = id;
        estado.avisos[static_cast<size_t>(pos)] = editado;
        responder_json(res, aviso_a_json(estado, editado));
    });

    svr.Delete(R"(/api/avisos/(\d+))", [&estado](const httplib::Request& req, httplib::Response& res) {
        const int id = id_de_url(req.matches[1].str());
        std::lock_guard<std::mutex> candado(estado.mtx);
        const int pos = buscar(estado, id);
        if (pos < 0) {
            responder_error(res, "No existe el aviso " + req.matches[1].str() + ".", 404);
            return;
        }
        estado.avisos.erase(estado.avisos.begin() + pos);
        json datos = json::object();
        datos["ok"] = true;
        responder_json(res, datos);
    });
}
