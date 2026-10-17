#pragma once
#include <vector>

#include "estado.hpp"
#include "httplib.h"

using Matriz = std::vector<std::vector<double>>;

Matriz transponer(const Matriz& a);                                  
Matriz multiplicar(const Matriz& a, const Matriz& b);                 
std::vector<double> por_vector(const Matriz& a, const std::vector<double>& v);  

void registrar_rutas_algebra(httplib::Server& svr, Estado& estado);
