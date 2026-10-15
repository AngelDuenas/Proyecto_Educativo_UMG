// DUEÑO: P1 · lógica del estado compartido
#include "estado.hpp"

#include <algorithm>
#include <cctype>

Estado& estado_global() {
    static Estado instancia;
    return instancia;
}

void inicializar_matrices(Estado& e) {
    for (int i = 0; i < MAX_ESTUDIANTES; ++i) {
        for (int j = 0; j < MAX_CURSOS; ++j)  e.notas[i][j] = SIN_NOTA;
        for (int d = 0; d < DIAS_SEMANA; ++d) e.asistencia[i][d] = 1;
    }
    for (int i = 0; i < MAX_CURSOS; ++i)
        for (int j = 0; j < MAX_CURSOS; ++j) e.adyacencia[i][j] = 0;
}

namespace {

struct DatosCurso { const char* codigo; const char* nombre; int creditos; int semestre; };
const DatosCurso CURSOS_EJEMPLO[] = {
    {"MAT101", "Matemática I", 5, 1},
    {"PRG101", "Introducción a la Programación", 5, 1},
    {"LOG101", "Lógica de Sistemas", 4, 1},
    {"MAT102", "Matemática II", 5, 2},
    {"PRG102", "Programación I", 5, 2},
    {"FIS101", "Física I", 4, 2},
    {"ALG201", "Álgebra Lineal", 5, 3},
    {"MDS201", "Matemática Discreta", 5, 3},
    {"PRG201", "Programación II", 5, 3},
    {"EDD301", "Estructura de Datos", 5, 4},
};

const char* NOMBRES_EJEMPLO[] = {
    "Ana Lucía Pérez", "Bryan Estuardo López", "Carla Sofía Méndez", "Diego Alejandro Ramírez",
    "Elena María Castillo", "Fernando José Ajú", "Gabriela Andrea Xol", "Héctor Manuel Ortiz",
};

struct Arista { const char* de; const char* a; };   // "de" es prerrequisito de "a"
const Arista PRERREQUISITOS_EJEMPLO[] = {
    {"MAT101", "MAT102"}, {"MAT101", "FIS101"}, {"MAT102", "ALG201"}, {"LOG101", "MDS201"},
    {"MAT102", "MDS201"}, {"PRG101", "PRG102"}, {"PRG102", "PRG201"}, {"MDS201", "EDD301"},
    {"PRG201", "EDD301"},
};

// Deja una fila de notas/asistencia como "vacía".
void limpiar_fila(Estado& e, size_t fila) {
    for (int j = 0; j < MAX_CURSOS; ++j)  e.notas[fila][j] = SIN_NOTA;
    for (int d = 0; d < DIAS_SEMANA; ++d) e.asistencia[fila][d] = 1;
}

}  // namespace

void reconstruir_adyacencia(Estado& e) {
    for (int i = 0; i < MAX_CURSOS; ++i)
        for (int j = 0; j < MAX_CURSOS; ++j) e.adyacencia[i][j] = 0;
    for (const Arista& a : PRERREQUISITOS_EJEMPLO) {
        const int i = indice_curso(e, a.de);
        const int j = indice_curso(e, a.a);
        if (i >= 0 && j >= 0) e.adyacencia[i][j] = 1;
    }
}

void cargar_datos_ejemplo(Estado& e) {
    e.estudiantes.clear();
    e.cursos.clear();
    inicializar_matrices(e);

    for (const DatosCurso& c : CURSOS_EJEMPLO) {
        CursoEstado curso;
        curso.codigo = c.codigo; curso.nombre = c.nombre; curso.creditos = c.creditos; curso.semestre = c.semestre;
        e.cursos.push_back(curso);
    }
    for (int i = 0; i < 8; ++i) {
        Estudiante s;
        s.carnet = "0901-26-" + std::to_string(1001 + i);
        s.nombre = NOMBRES_EJEMPLO[i];
        s.carrera = "Ingeniería en Sistemas";
        s.semestre = 2;
        e.estudiantes.push_back(s);
        // Mismos valores que el "modo demo" del frontend, para que se vea igual.
        for (int j = 0; j < 10; ++j) e.notas[i][j] = static_cast<float>(42 + ((i * 37 + j * 53 + 11) % 59));
        for (int d = 0; d < DIAS_SEMANA; ++d) e.asistencia[i][d] = ((i * 3 + d * 7) % 9 == 0) ? 0 : 1;
    }
    reconstruir_adyacencia(e);
}

// Búsqueda lineal: recorre el vector hasta encontrar el carnet.
int indice_estudiante(const Estado& e, const std::string& carnet) {
    for (size_t i = 0; i < e.estudiantes.size(); ++i)
        if (e.estudiantes[i].carnet == carnet) return static_cast<int>(i);
    return -1;
}

int indice_curso(const Estado& e, const std::string& codigo) {
    for (size_t i = 0; i < e.cursos.size(); ++i)
        if (e.cursos[i].codigo == codigo) return static_cast<int>(i);
    return -1;
}

Resultado agregar_estudiante(Estado& e, const Estudiante& nuevo) {
    if (indice_estudiante(e, nuevo.carnet) >= 0) return Resultado::DUPLICADO;
    if (e.estudiantes.size() >= static_cast<size_t>(MAX_ESTUDIANTES)) return Resultado::LLENO;
    e.estudiantes.push_back(nuevo);
    limpiar_fila(e, e.estudiantes.size() - 1);   // la fila nueva empieza vacía
    return Resultado::OK;
}

// Al eliminar la fila `idx`, todas las de abajo "suben" una posición en AMBAS
// matrices; así cada fila sigue perteneciendo al estudiante correcto.
Resultado quitar_estudiante(Estado& e, const std::string& carnet) {
    const int pos = indice_estudiante(e, carnet);
    if (pos < 0) return Resultado::NO_EXISTE;
    const size_t idx = static_cast<size_t>(pos);
    const size_t total = e.estudiantes.size();
    for (size_t fila = idx; fila + 1 < total; ++fila) {
        for (int j = 0; j < MAX_CURSOS; ++j)  e.notas[fila][j] = e.notas[fila + 1][j];
        for (int d = 0; d < DIAS_SEMANA; ++d) e.asistencia[fila][d] = e.asistencia[fila + 1][d];
    }
    limpiar_fila(e, total - 1);
    e.estudiantes.erase(e.estudiantes.begin() + static_cast<std::ptrdiff_t>(idx));
    e.inscripciones.erase(std::remove_if(e.inscripciones.begin(), e.inscripciones.end(),
                                         [&carnet](const Inscripcion& x) { return x.carnet == carnet; }),
                          e.inscripciones.end());
    return Resultado::OK;
}

Resultado agregar_curso(Estado& e, const CursoEstado& nuevo) {
    if (indice_curso(e, nuevo.codigo) >= 0) return Resultado::DUPLICADO;
    if (e.cursos.size() >= static_cast<size_t>(MAX_CURSOS)) return Resultado::LLENO;
    e.cursos.push_back(nuevo);
    const size_t col = e.cursos.size() - 1;
    for (int i = 0; i < MAX_ESTUDIANTES; ++i) e.notas[i][col] = SIN_NOTA;
    for (int k = 0; k < MAX_CURSOS; ++k) { e.adyacencia[col][k] = 0; e.adyacencia[k][col] = 0; }
    return Resultado::OK;
}

// Quitar un curso = quitar una COLUMNA de notas y una fila/columna de prerrequisitos.
Resultado quitar_curso(Estado& e, const std::string& codigo) {
    const int pos = indice_curso(e, codigo);
    if (pos < 0) return Resultado::NO_EXISTE;
    const size_t idx = static_cast<size_t>(pos);
    const size_t total = e.cursos.size();

    for (int i = 0; i < MAX_ESTUDIANTES; ++i) {
        for (size_t j = idx; j + 1 < total; ++j) e.notas[i][j] = e.notas[i][j + 1];
        e.notas[i][total - 1] = SIN_NOTA;
    }
    for (size_t r = idx; r + 1 < total; ++r)                 // las filas de abajo suben
        for (int c = 0; c < MAX_CURSOS; ++c) e.adyacencia[r][c] = e.adyacencia[r + 1][c];
    for (int r = 0; r < MAX_CURSOS; ++r)                     // las columnas de la derecha se corren a la izquierda
        for (size_t c = idx; c + 1 < total; ++c) e.adyacencia[r][c] = e.adyacencia[r][c + 1];
    for (int k = 0; k < MAX_CURSOS; ++k) { e.adyacencia[total - 1][k] = 0; e.adyacencia[k][total - 1] = 0; }

    e.cursos.erase(e.cursos.begin() + static_cast<std::ptrdiff_t>(idx));

    // El curso deja de existir también en los módulos que lo mencionan por código.
    e.inscripciones.erase(std::remove_if(e.inscripciones.begin(), e.inscripciones.end(),
                                         [&codigo](const Inscripcion& x) { return x.curso == codigo; }),
                          e.inscripciones.end());
    for (auto& dia : e.horario)
        for (std::vector<std::string>& bloque : dia)
            bloque.erase(std::remove(bloque.begin(), bloque.end(), codigo), bloque.end());
    for (Aviso& a : e.avisos)
        if (a.curso == codigo) a.curso.clear();
    return Resultado::OK;
}

// Pasa a minúsculas y quita las tildes de las letras más comunes en español (UTF-8).
std::string normalizar_texto(const std::string& texto) {
    std::string r;
    r.reserve(texto.size());
    for (size_t i = 0; i < texto.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(texto[i]);
        if (c == 0xC3 && i + 1 < texto.size()) {
            char reemplazo = 0;
            switch (static_cast<unsigned char>(texto[i + 1])) {
                case 0x81: case 0xA1: reemplazo = 'a'; break;   // Á á
                case 0x89: case 0xA9: reemplazo = 'e'; break;   // É é
                case 0x8D: case 0xAD: reemplazo = 'i'; break;   // Í í
                case 0x93: case 0xB3: reemplazo = 'o'; break;   // Ó ó
                case 0x9A: case 0xBA: reemplazo = 'u'; break;   // Ú ú
                case 0x9C: case 0xBC: reemplazo = 'u'; break;   // Ü ü
                case 0x91: case 0xB1: reemplazo = 'n'; break;   // Ñ ñ
                default: break;
            }
            if (reemplazo != 0) { r += reemplazo; ++i; continue; }
        }
        r += static_cast<char>(std::tolower(c));
    }
    return r;
}

namespace {
// ¿`a` debe ir DESPUÉS de `b` en la lista ordenada?
bool va_despues(const Estudiante& a, const Estudiante& b, const std::string& criterio) {
    const std::string na = normalizar_texto(a.nombre);
    const std::string nb = normalizar_texto(b.nombre);
    if (criterio == "nombre-desc") return na < nb;
    if (criterio == "carnet")      return a.carnet > b.carnet;
    if (criterio == "semestre")    return a.semestre != b.semestre ? a.semestre > b.semestre : na > nb;
    return na > nb;   // "nombre" (por defecto)
}
}  // namespace

// Ordenamiento por inserción (escrito a mano): toma cada elemento y lo inserta en
// su lugar dentro de la parte que ya está ordenada. Es estable.
void ordenar_estudiantes(std::vector<Estudiante>& lista, const std::string& criterio) {
    for (size_t i = 1; i < lista.size(); ++i) {
        const Estudiante clave = lista[i];
        size_t j = i;
        while (j > 0 && va_despues(lista[j - 1], clave, criterio)) {
            lista[j] = lista[j - 1];   // desplaza a la derecha
            --j;
        }
        lista[j] = clave;
    }
}
