#include "respuestas.hpp"

#include <cmath>

namespace {
// error_handler_t::replace evita que un texto con caracteres raros haga fallar dump().
std::string a_texto(const json& j) {
    return j.dump(-1, ' ', false, json::error_handler_t::replace);
}
}  // namespace

void responder_ok(httplib::Response& res, const json& datos, int estado_http) {
    json cuerpo = json::object();
    cuerpo["ok"] = true;
    cuerpo["datos"] = datos;
    res.status = estado_http;
    res.set_content(a_texto(cuerpo), "application/json; charset=utf-8");
}

void responder_error(httplib::Response& res, const std::string& mensaje, int estado_http) {
    json cuerpo = json::object();
    cuerpo["ok"] = false;
    cuerpo["error"] = mensaje;
    res.status = estado_http;
    res.set_content(a_texto(cuerpo), "application/json; charset=utf-8");
}

bool leer_cuerpo_json(const httplib::Request& req, httplib::Response& res, json& cuerpo) {
    cuerpo = json::parse(req.body, nullptr, false);   // false = no lanzar excepción si falla
    if (cuerpo.is_discarded() || !cuerpo.is_object()) {
        responder_error(res, "El cuerpo de la petición debe ser un objeto JSON válido.", 400);
        return false;
    }
    return true;
}

double redondear(double valor, int decimales) {
    const double factor = std::pow(10.0, decimales);
    return std::round(valor * factor) / factor;
}
