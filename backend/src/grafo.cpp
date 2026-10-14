#include "grafo.hpp"

#include <queue>
#include <vector>

#include "../third_party/json.hpp"
#include "docentes.hpp"

using json = nlohmann::json;

const int MAX_CURSOS_GRAFO = 50;

// Prerrequisitos semilla: { prerrequisito, curso que desbloquea }.
// Solo se usan las aristas cuyos dos cursos existen.
static const char* ARISTAS[][2] = {
    {"MAT101", "MAT102"}, {"MAT101", "FIS101"}, {"MAT102", "ALG201"},
    {"LOG101", "MDS201"}, {"MAT102", "MDS201"}, {"PRG101", "PRG102"},
    {"PRG102", "PRG201"}, {"MDS201", "EDD301"}, {"PRG201", "EDD301"},
};

struct Grafo {
    std::vector<std::string> codigos;
    std::vector<std::string> nombres;
    int adyacencia[MAX_CURSOS_GRAFO][MAX_CURSOS_GRAFO] = {};
};

static int indiceCurso(const Grafo& g, const std::string& codigo) {
    for (size_t i = 0; i < g.codigos.size(); i++)
        if (g.codigos[i] == codigo) return static_cast<int>(i);
    return -1;
}

// Se arma en cada llamada a partir de los cursos actuales, así refleja altas y bajas.
static Grafo construirGrafo() {
    Grafo g;
    json cursos = json::parse(obtenerCursosJSON(), nullptr, false);
    if (cursos.is_array()) {
        for (const auto& c : cursos) {
            if (g.codigos.size() >= static_cast<size_t>(MAX_CURSOS_GRAFO)) break;
            g.codigos.push_back(c.value("codigo", ""));
            g.nombres.push_back(c.value("nombre", ""));
        }
    }
    for (const auto& arista : ARISTAS) {
        int a = indiceCurso(g, arista[0]);
        int b = indiceCurso(g, arista[1]);
        if (a >= 0 && b >= 0) g.adyacencia[a][b] = 1;
    }
    return g;
}

static void dfs(const Grafo& g, int actual, std::vector<bool>& visitado, std::vector<int>& orden) {
    visitado[actual] = true;
    orden.push_back(actual);
    for (size_t j = 0; j < g.codigos.size(); j++)
        if (g.adyacencia[actual][j] == 1 && !visitado[j]) dfs(g, static_cast<int>(j), visitado, orden);
}

std::string obtenerGrafoJSON() {
    Grafo g = construirGrafo();
    const size_t n = g.codigos.size();
    json nodos = json::array(), matriz = json::array();
    for (size_t i = 0; i < n; i++) {
        nodos.push_back({{"codigo", g.codigos[i]}, {"nombre", g.nombres[i]}});
        json fila = json::array();
        for (size_t j = 0; j < n; j++) fila.push_back(g.adyacencia[i][j]);
        matriz.push_back(fila);
    }
    return json{{"nodos", nodos}, {"matriz", matriz}}.dump();
}

bool recorrerGrafoJSON(const std::string& origen, bool anchura, std::string& salida) {
    Grafo g = construirGrafo();
    const int inicio = indiceCurso(g, origen);
    if (inicio < 0) return false;

    std::vector<bool> visitado(g.codigos.size(), false);
    std::vector<int> orden;

    if (anchura) {
        std::queue<int> cola;
        cola.push(inicio);
        visitado[inicio] = true;
        while (!cola.empty()) {
            int actual = cola.front();
            cola.pop();
            orden.push_back(actual);
            for (size_t j = 0; j < g.codigos.size(); j++) {
                if (g.adyacencia[actual][j] == 1 && !visitado[j]) {
                    visitado[j] = true;
                    cola.push(static_cast<int>(j));
                }
            }
        }
    } else {
        dfs(g, inicio, visitado, orden);
    }

    json codigos = json::array();
    for (int i : orden) codigos.push_back(g.codigos[i]);
    salida = json{{"origen", origen}, {"orden", codigos}}.dump();
    return true;
}
