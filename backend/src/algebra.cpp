// Módulo Álgebra lineal aplicada. Formato definido por vista-algebra.js.
// Las operaciones están escritas a mano (ciclos anidados) para que se vea cómo funcionan.
#include "algebra.hpp"

#include <algorithm>
#include <mutex>

#include "algoritmos.hpp"
#include "respuestas.hpp"

namespace {

constexpr double PESO_NOTAS = 0.9;
constexpr double PESO_ASISTENCIA = 0.1;

json redondear_vector(const std::vector<double>& v) {
    json r = json::array();
    for (double x : v) r.push_back(redondear(x));
    return r;
}

json redondear_matriz(const Matriz& m) {
    json r = json::array();
    for (const auto& fila : m) r.push_back(redondear_vector(fila));
    return r;
}

}  // namespace

Matriz transponer(const Matriz& a) {
    if (a.empty()) return {};
    Matriz t(a[0].size(), std::vector<double>(a.size(), 0.0));
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = 0; j < a[i].size(); ++j) t[j][i] = a[i][j];
    return t;
}

Matriz multiplicar(const Matriz& a, const Matriz& b) {
    if (a.empty() || b.empty()) return {};
    const size_t filas = a.size(), comun = b.size(), columnas = b[0].size();
    Matriz r(filas, std::vector<double>(columnas, 0.0));
    for (size_t i = 0; i < filas; ++i)
        for (size_t j = 0; j < columnas; ++j)
            for (size_t k = 0; k < comun; ++k) r[i][j] += a[i][k] * b[k][j];
    return r;
}

std::vector<double> por_vector(const Matriz& a, const std::vector<double>& v) {
    std::vector<double> r(a.size(), 0.0);
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = 0; j < v.size() && j < a[i].size(); ++j) r[i] += a[i][j] * v[j];
    return r;
}

void registrar_rutas_algebra(httplib::Server& svr, Estado& estado) {
    svr.Get("/api/algebra", [&estado](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> candado(estado.mtx);
        const size_t n = std::min(estado.estudiantes.size(), static_cast<size_t>(MAX_ESTUDIANTES));
        const size_t m = std::min(estado.cursos.size(), static_cast<size_t>(MAX_CURSOS));

        // A: matriz de notas (una celda sin nota cuenta como 0 en los productos y se marca aparte).
        Matriz a(n, std::vector<double>(m, 0.0));
        json a_json = json::array();
        for (size_t i = 0; i < n; ++i) {
            json fila = json::array();
            for (size_t j = 0; j < m; ++j) {
                const float nota = estado.notas[i][j];
                a[i][j] = nota < 0.0f ? 0.0 : static_cast<double>(nota);
                fila.push_back(nota < 0.0f ? json(nullptr) : json(redondear(nota)));
            }
            a_json.push_back(fila);
        }

        // Vector de créditos c y producto A·c (suma de nota × créditos de cada estudiante).
        std::vector<double> c(m, 0.0);
        for (size_t j = 0; j < m; ++j) c[j] = estado.cursos[j].creditos;
        const std::vector<double> ac = por_vector(a, c);

        // Promedio ponderado = (A·c)_i / créditos de los cursos en los que tiene nota.
        std::vector<double> creditos_con_nota(n, 0.0), ponderado(n, 0.0), simple(n, 0.0), asistencia(n, 0.0), combinado(n, 0.0);
        for (size_t i = 0; i < n; ++i) {
            int con_nota = 0;
            for (size_t j = 0; j < m; ++j)
                if (estado.notas[i][j] >= 0.0f) {
                    creditos_con_nota[i] += c[j];
                    simple[i] += a[i][j];
                    ++con_nota;
                }
            ponderado[i] = creditos_con_nota[i] > 0 ? ac[i] / creditos_con_nota[i] : 0.0;
            simple[i] = con_nota > 0 ? simple[i] / con_nota : 0.0;
            asistencia[i] = porcentaje_estudiante(estado, i);
            // Combinación lineal de dos vectores: 0.9·ponderado + 0.1·asistencia.
            combinado[i] = PESO_NOTAS * ponderado[i] + PESO_ASISTENCIA * asistencia[i];
        }

        // Promedio por curso: Aᵀ·1 suma cada columna; se divide entre las notas que tiene el curso.
        const Matriz at = transponer(a);
        const std::vector<double> sumas_curso = por_vector(at, std::vector<double>(n, 1.0));
        std::vector<double> promedio_curso(m, 0.0);
        for (size_t j = 0; j < m; ++j) {
            int con_nota = 0;
            for (size_t i = 0; i < n; ++i)
                if (estado.notas[i][j] >= 0.0f) ++con_nota;
            promedio_curso[j] = con_nota > 0 ? sumas_curso[j] / con_nota : 0.0;
        }

        // Matriz de adyacencia M y M² = M·M: M²[i][j] cuenta las cadenas i → k → j de prerrequisitos.
        Matriz mady(m, std::vector<double>(m, 0.0));
        for (size_t i = 0; i < m; ++i)
            for (size_t j = 0; j < m; ++j) mady[i][j] = estado.adyacencia[i][j];
        const Matriz m2 = multiplicar(mady, mady);
        json cadenas = json::array();
        for (size_t i = 0; i < m; ++i)
            for (size_t j = 0; j < m; ++j) {
                if (m2[i][j] <= 0.0) continue;
                json pasa = json::array();
                for (size_t k = 0; k < m; ++k)
                    if (mady[i][k] > 0.0 && mady[k][j] > 0.0) pasa.push_back(estado.cursos[k].codigo);
                json x = json::object();
                x["desde"] = estado.cursos[i].codigo;
                x["pasaPor"] = pasa;
                x["hasta"] = estado.cursos[j].codigo;
                cadenas.push_back(x);
            }

        json estudiantes = json::array(), cursos = json::array(), codigos = json::array();
        for (size_t i = 0; i < n; ++i) {
            json x = json::object();
            x["carnet"] = estado.estudiantes[i].carnet;
            x["nombre"] = estado.estudiantes[i].nombre;
            estudiantes.push_back(x);
        }
        for (size_t j = 0; j < m; ++j) {
            json x = json::object();
            x["codigo"] = estado.cursos[j].codigo;
            x["nombre"] = estado.cursos[j].nombre;
            x["creditos"] = estado.cursos[j].creditos;
            cursos.push_back(x);
            codigos.push_back(estado.cursos[j].codigo);
        }

        json combinacion = json::object();
        combinacion["pesoNotas"] = PESO_NOTAS;
        combinacion["pesoAsistencia"] = PESO_ASISTENCIA;
        combinacion["asistencia"] = redondear_vector(asistencia);
        combinacion["resultado"] = redondear_vector(combinado);

        json adyacencia = json::object();
        adyacencia["codigos"] = codigos;
        adyacencia["M"] = redondear_matriz(mady);
        adyacencia["M2"] = redondear_matriz(m2);
        adyacencia["cadenas"] = cadenas;

        json datos = json::object();
        datos["estudiantes"] = estudiantes;
        datos["cursos"] = cursos;
        datos["A"] = a_json;
        // Aᵀ se muestra con las mismas celdas vacías que A (en los productos cuentan como 0).
        json transpuesta = json::array();
        for (size_t j = 0; j < m; ++j) {
            json fila = json::array();
            for (size_t i = 0; i < n; ++i) fila.push_back(a_json[i][j]);
            transpuesta.push_back(fila);
        }
        datos["transpuesta"] = transpuesta;
        datos["creditos"] = redondear_vector(c);
        datos["productoAc"] = redondear_vector(ac);
        datos["creditosConNota"] = redondear_vector(creditos_con_nota);
        datos["promedioPonderado"] = redondear_vector(ponderado);
        datos["promedioSimple"] = redondear_vector(simple);
        datos["promedioCursos"] = redondear_vector(promedio_curso);
        datos["combinacion"] = combinacion;
        datos["adyacencia"] = adyacencia;
        responder_json(res, datos);
    });
}
