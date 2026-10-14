#ifndef RESPUESTAS_HPP
#define RESPUESTAS_HPP

// Helpers para responder siempre igual a lo que espera frontend/js/api.js:
//   - Éxito: el JSON directo (lista u objeto), sin envolver.
//   - Error: código HTTP >= 400 y cuerpo { "error": "Mensaje legible" }.

#include <string>

#include "../third_party/httplib.h"
#include "../third_party/json.hpp"

void responderJSON(httplib::Response& res, const std::string& cuerpo, int estado = 200);
void responderJSON(httplib::Response& res, const nlohmann::json& cuerpo, int estado = 200);
void responderError(httplib::Response& res, const std::string& mensaje, int estado = 400);

// Lee el cuerpo de la petición como objeto JSON. Si no es válido responde 400 y devuelve false.
bool leerCuerpo(const httplib::Request& req, httplib::Response& res, nlohmann::json& destino);

// Lectura tolerante de campos: acepta números que llegan como texto y al revés.
std::string campoTexto(const nlohmann::json& j, const std::string& clave);
int campoEntero(const nlohmann::json& j, const std::string& clave, int porDefecto = 0);

#endif
