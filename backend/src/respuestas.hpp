// Helpers compartidos para responder en JSON. (Base del equipo)
#pragma once
#include <string>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

// Respuesta correcta:  { "ok": true, "datos": ... }
void responder_ok(httplib::Response& res, const json& datos, int estado_http = 200);

// Respuesta de error:  { "ok": false, "error": "mensaje" }
void responder_error(httplib::Response& res, const std::string& mensaje, int estado_http = 400);

// Lee el cuerpo de una petición POST/PUT como objeto JSON.
// Si no es un JSON válido, responde 400 y devuelve false (el handler debe hacer return).
bool leer_cuerpo_json(const httplib::Request& req, httplib::Response& res, json& cuerpo);

// Redondea a N decimales (para que 85.3 no salga como 85.30000305...).
double redondear(double valor, int decimales = 2);
