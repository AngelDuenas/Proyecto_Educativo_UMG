#include "respuestas.hpp"

void responder_json(httplib::Response& res, const json& datos, int estado_http) {
    res.status = estado_http;
    // error_handler_t::replace evita que un texto con caracteres raros haga fallar dump().
    res.set_content(datos.dump(-1, ' ', false, json::error_handler_t::replace), "application/json; charset=utf-8");
}

void responder_json_texto(httplib::Response& res, const std::string& json_ya_armado, int estado_http) {
    res.status = estado_http;
    res.set_content(json_ya_armado, "application/json; charset=utf-8");
}

void responder_error(httplib::Response& res, const std::string& mensaje, int estado_http) {
    json cuerpo = json::object();
    cuerpo["error"] = mensaje;
    responder_json(res, cuerpo, estado_http);
}

bool leer_cuerpo_json(const httplib::Request& req, httplib::Response& res, json& cuerpo) {
    cuerpo = json::parse(req.body, nullptr, false);   // false = no lanzar excepción si falla
    if (cuerpo.is_discarded() || !cuerpo.is_object()) {
        responder_error(res, "El cuerpo de la petición debe ser un objeto JSON válido.", 400);
        return false;
    }
    return true;
}
