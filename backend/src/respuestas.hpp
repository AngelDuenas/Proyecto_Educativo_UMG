// Helpers compartidos para responder en JSON con el formato que espera el frontend:
//   éxito -> el JSON de los datos directamente (sin envoltorio)
//   error -> código 4xx/5xx con  { "error": "mensaje para el usuario" }
#pragma once
#include <string>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

void responder_json(httplib::Response& res, const json& datos, int estado_http = 200);
void responder_json_texto(httplib::Response& res, const std::string& json_ya_armado, int estado_http = 200);
void responder_error(httplib::Response& res, const std::string& mensaje, int estado_http = 400);

// Lee el cuerpo de un POST/PUT como objeto JSON. Si no es válido, responde 400 y devuelve false.
bool leer_cuerpo_json(const httplib::Request& req, httplib::Response& res, json& cuerpo);
