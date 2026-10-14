#include "respuestas.hpp"

using json = nlohmann::json;

void responderJSON(httplib::Response& res, const std::string& cuerpo, int estado) {
    res.status = estado;
    res.set_content(cuerpo, "application/json; charset=utf-8");
}

void responderJSON(httplib::Response& res, const json& cuerpo, int estado) {
    responderJSON(res, cuerpo.dump(), estado);
}

void responderError(httplib::Response& res, const std::string& mensaje, int estado) {
    responderJSON(res, json{{"error", mensaje}}, estado);
}

bool leerCuerpo(const httplib::Request& req, httplib::Response& res, json& destino) {
    destino = json::parse(req.body, nullptr, false);  // sin excepciones
    if (destino.is_discarded() || !destino.is_object()) {
        responderError(res, "El cuerpo de la petición debe ser un objeto JSON válido", 400);
        return false;
    }
    return true;
}

std::string campoTexto(const json& j, const std::string& clave) {
    if (!j.contains(clave) || j[clave].is_null()) return "";
    if (j[clave].is_string()) return j[clave].get<std::string>();
    return j[clave].dump();
}

int campoEntero(const json& j, const std::string& clave, int porDefecto) {
    if (!j.contains(clave)) return porDefecto;
    const json& v = j[clave];
    if (v.is_number()) return v.get<int>();
    if (v.is_boolean()) return v.get<bool>() ? 1 : 0;
    if (v.is_string()) {
        try {
            return std::stoi(v.get<std::string>());
        } catch (...) {
            return porDefecto;
        }
    }
    return porDefecto;
}
