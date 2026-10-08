# 🧭 Operación Pizarrón — Instrucciones para Claude Code

> **Proyecto:** Sistema Integral de Gestión Escolar
> **Stack:** C++17 (backend, API HTTP/JSON) + HTML/CSS/JavaScript (frontend)
> **Equipo:** 5 personas · **Plazo:** 7 días
> **Objetivo de este archivo:** que Claude Code genere la **base completa del repositorio** (estructura, archivos, stubs que compilan, datos semilla y documentación) para que el equipo solo tenga que subirlo y empezar a trabajar.

---

## 👤 Para el humano: cómo usar este archivo

1. Crea una carpeta vacía para el repo y ábrela con Claude Code (`claude` en la terminal).
2. Copia este archivo dentro de esa carpeta.
3. Pega este mensaje en Claude Code:

   ```
   Lee INSTRUCCIONES_CLAUDE_CODE.md completo y ejecuta todas las fases en orden.
   No me hagas preguntas: si algo es ambiguo, elige la opción más simple y anótala en docs/DECISIONES.md.
   Al terminar, muéstrame el resumen final que pide la sección 12.
   ```

4. Cuando termine, revisa el resumen, prueba el servidor y sube el repo (`git remote add origin <url>` y `git push -u origin main`).

---

## 0. Reglas para Claude Code (léelas primero)

- Eres quien **arma el andamiaje**. No implementes la lógica final de cada módulo: el equipo la hará durante la semana. Sí debes entregar una base que **compila, arranca y responde**.
- Todo el código, comentarios, nombres de variables, funciones, archivos y documentación van en **español** (sin tildes ni ñ en identificadores y nombres de archivo; sí se permiten en textos y comentarios).
- No uses base de datos. Los datos viven en memoria (vectores y matrices) y se guardan en archivos JSON dentro de `data/`.
- No instales dependencias globales. Las librerías se copian dentro del repo (`backend/third_party/`).
- No hagas `git push` ni agregues remotos. Solo `git init`, commits y ramas locales.
- Cada archivo que pertenezca a una persona del equipo debe llevar arriba un comentario con su **dueño** y sus **tareas**, por ejemplo: `// DUEÑO: P2 · Tareas: T11, T15, T18`.
- Donde falte lógica, deja marcadores con este formato exacto, para poder buscarlos con `grep`: `// TODO(P2, T11): descripción corta`.
- Si una descarga falla o no hay internet, no inventes el contenido de `httplib.h` ni de `json.hpp`: detente en ese paso, déjalo documentado en `docs/DECISIONES.md` y continúa con el resto.

---

## 1. Contexto del proyecto

Sistema de gestión escolar donde las estructuras de álgebra lineal y matemática discreta **tienen una función real**, no están metidas "porque la tarea lo pide":

| Estructura | Dónde se usa en el sistema |
|---|---|
| **Vectores** (`std::vector`) | Estudiantes, docentes y cursos: registrar, buscar, ordenar, editar, eliminar |
| **Matrices** | `notas[estudiante][curso]` (calificaciones y estadísticas), `asistencia[estudiante][día]` (porcentajes) y `adyacencia[curso][curso]` (prerrequisitos) |
| **Árbol** | Estructura académica: Colegio → Básicos / Diversificado → grados → secciones (`struct Nodo` con `vector<Nodo*> hijos`) |
| **Grafo** | Prerrequisitos entre cursos (vértice = curso, arista = prerrequisito), con **BFS** y **DFS** |

Arquitectura:

```
FRONTEND (HTML + CSS + JS)  ──HTTP/JSON──►  BACKEND C++ (cpp-httplib)
                                                 │
                                   vectores · matrices · árbol · grafo
                                                 │
                                       archivos JSON en data/
```

El backend también **sirve el frontend como archivos estáticos** (así no hay problemas de CORS y se abre todo en `http://localhost:8080`).

---

## 2. Equipo y reparto por vistas

El trabajo se reparte **por vistas completas** (lógica en C++ + endpoints + pantalla), no por capas. Cada persona puede probar y mostrar lo suyo sola.

| Persona | Rol | Vistas / módulos que construye |
|---|---|---|
| **P1** | ⚙️ Líder de Integración + Estudiantes | Contrato de API, servidor base, módulo **Estudiantes**, revisión/merge e integración final |
| **P2** | 🧮 Maestro de Matrices | **Calificaciones y estadísticas** (matriz de notas) y **persistencia** en archivos |
| **P3** | 🌳 Cartógrafo de Grafos y Árboles | **Estructura académica (árbol)** y **Prerrequisitos (grafo)** con BFS/DFS |
| **P4** | 🎨 Diseñador del Shell + Asistencia | **Shell** compartido (menú, CSS, `api.js`), **Asistencia**, **Reportes** y pulido final |
| **P5** | 🛡️ Guardián de Datos y Calidad | structs/vectores, **Docentes y Cursos**, pruebas, manual y repo Git |

Tareas por persona (los IDs coinciden con el plan de trabajo en Excel):

- **P1:** T03 contrato de API · T07 servidor base · T10 estudiantes (lógica + endpoints) · T13 vista Estudiantes · T22 hito integración 1 · T28 revisión y merge · T29 integración final · T38 code freeze y entrega
- **P2:** T05 diseño de la matriz de notas · T11 matriz de calificaciones · T15 estadísticas · T18 endpoints de notas y estadísticas · T23 vista Calificaciones · T24 persistencia · T33 doc de álgebra lineal · T34 datos de demostración
- **P3:** T06 diseño de árbol y grafo · T12 árbol académico · T16 vista Estructura académica · T21 grafo + matriz de adyacencia · T26 BFS y DFS · T31 vista Prerrequisitos · T35 doc de matemática discreta
- **P4:** T04 wireframes · T09 shell compartido · T17 asistencia (matriz + endpoints) · T20 vista Asistencia · T25 reportes · T30 pulido de UI · T36 diapositivas
- **P5:** T01 kickoff · T02 repo y guía de compilación · T08 structs y vectores · T14 docentes y cursos · T19 vista Docentes y Cursos · T27 pruebas · T32 manual · T37 ensayo

> El andamiaje que generas **adelanta en versión mínima** las tareas T02, T03, T07, T08 y T09. Sus dueños las revisan y completan; no las des por terminadas.

Colores por persona (úsalos en el menú del shell y en `docs/REPARTO.md`):
`P1 #2E75B6` · `P2 #ED7D31` · `P3 #3E9B4F` · `P4 #8E6CC9` · `P5 #D9455F`

---

## 3. Estructura de carpetas a crear

```
sistema-gestion-escolar/
├── README.md
├── CLAUDE.md                      # contexto permanente para Claude Code (ver sección 9)
├── .gitignore
├── CMakeLists.txt
├── Makefile
├── backend/
│   ├── src/
│   │   ├── main.cpp               # SOLO crea el servidor y registra rutas
│   │   ├── estado.hpp / estado.cpp         # structs, constantes y estado global (P5 + P1)
│   │   ├── respuestas.hpp / respuestas.cpp # helpers de respuesta JSON
│   │   ├── persistencia.hpp / persistencia.cpp   # guardar/cargar JSON (P2)
│   │   ├── estudiantes.hpp / estudiantes.cpp     # P1
│   │   ├── docentes_cursos.hpp / docentes_cursos.cpp   # P5
│   │   ├── notas.hpp / notas.cpp                 # P2
│   │   ├── asistencia.hpp / asistencia.cpp       # P4
│   │   ├── reportes.hpp / reportes.cpp           # P4
│   │   ├── arbol.hpp / arbol.cpp                 # P3
│   │   └── grafo.hpp / grafo.cpp                 # P3
│   └── third_party/
│       ├── httplib.h              # cpp-httplib (una sola cabecera)
│       └── json.hpp               # nlohmann/json (una sola cabecera)
├── frontend/
│   ├── index.html                 # shell: menú lateral + contenedor de vistas (P4)
│   ├── css/estilos.css            # estilos comunes (P4)
│   ├── js/
│   │   ├── api.js                 # cliente de la API + utilidades (P4)
│   │   └── app.js                 # router por hash y carga de vistas (P4)
│   └── vistas/
│       ├── vista-estudiantes.html / .js          # P1
│       ├── vista-docentes-cursos.html / .js      # P5
│       ├── vista-calificaciones.html / .js       # P2
│       ├── vista-asistencia.html / .js           # P4
│       ├── vista-reportes.html / .js             # P4
│       ├── vista-arbol.html / .js                # P3
│       └── vista-grafo.html / .js                # P3
├── data/
│   ├── estudiantes.json
│   ├── docentes.json
│   ├── cursos.json
│   ├── notas.json
│   ├── asistencia.json
│   ├── grafo.json
│   ├── arbol.json
│   └── backup/.gitkeep
├── docs/
│   ├── API.md                     # contrato de la API (endpoints y JSON)
│   ├── REPARTO.md                 # reparto por persona, vistas y tareas
│   ├── GIT.md                     # flujo de trabajo con ramas y merge diario
│   ├── DECISIONES.md              # decisiones tomadas al generar la base
│   └── ARQUITECTURA.md            # diagrama y explicación de estructuras
└── tests/
    ├── smoke.sh                   # prueba rápida con curl
    └── casos_de_prueba.md         # plantilla de casos de prueba (P5)
```

---

## 4. Convenciones técnicas

**Compilación**
- C++17, `g++` (o MinGW en Windows). Flags: `-std=c++17 -O2 -Wall -Wextra -pthread`.
- En Windows hay que enlazar `ws2_32` y `wsock32`, y definir `_WIN32_WINNT=0x0A00`.
- El ejecutable queda en `build/gestion_escolar` (o `build/gestion_escolar.exe`) y **se ejecuta desde la raíz del repo**.
- Puerto por defecto `8080`; permite cambiarlo con la variable de entorno `PUERTO`.

**Librerías (copiar dentro del repo)**
- `httplib.h` desde `https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h`
- `json.hpp` desde `https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp`
- Verifica que cada archivo descargado no esté vacío y no sea una página HTML de error (debe contener `#ifndef CPPHTTPLIB_HTTPLIB_H` y `NLOHMANN_JSON_VERSION_MAJOR` respectivamente).

**Formato de todas las respuestas de la API**

```json
{ "ok": true,  "datos": { } }
{ "ok": false, "error": "Mensaje legible para el usuario" }
```

Códigos HTTP: `200` ok · `201` creado · `400` datos inválidos · `404` no existe · `500` error interno. El cuerpo de las peticiones POST/PUT es JSON.

**Nombres**
- Structs: `PascalCase` (`Estudiante`). Funciones y variables: `snake_case` (`calcular_promedio_estudiante`).
- Cada módulo expone **una** función de registro: `void registrar_rutas_<modulo>(httplib::Server& svr, Estado& estado);`
- `main.cpp` solo llama a esas funciones: así nadie edita el mismo archivo y se evitan conflictos de Git.

**Estructuras base (archivo `backend/src/estado.hpp`)** — créalo con este contenido (puedes mejorar detalles, pero conserva los nombres y las dimensiones):

```cpp
#pragma once
#include <mutex>
#include <string>
#include <vector>

constexpr int   MAX_ESTUDIANTES  = 100;
constexpr int   MAX_CURSOS       = 10;
constexpr int   DIAS_SEMANA      = 5;      // L M X J V
constexpr float SIN_NOTA         = -1.0f;  // valor centinela en la matriz de notas
constexpr float NOTA_APROBACION  = 60.0f;
constexpr int   SIN_REGISTRO     = -1;     // asistencia: 1 presente, 0 ausente, -1 sin registro

struct Estudiante { int id; std::string nombre; int edad; std::string grado; std::string seccion; };
struct Docente    { int id; std::string nombre; std::string especialidad; };
struct Curso      { int id; std::string nombre; int docente_id; };   // docente_id = -1 si no tiene

struct Nodo {                       // árbol de la estructura académica
    std::string nombre;
    std::vector<Nodo*> hijos;
};

struct Estado {
    std::vector<Estudiante> estudiantes;
    std::vector<Docente>    docentes;
    std::vector<Curso>      cursos;

    // Matrices. La FILA es la posición del estudiante en el vector `estudiantes`;
    // la COLUMNA es la posición del curso en el vector `cursos`.
    float notas[MAX_ESTUDIANTES][MAX_CURSOS];
    int   asistencia[MAX_ESTUDIANTES][DIAS_SEMANA];
    int   adyacencia[MAX_CURSOS][MAX_CURSOS];   // adyacencia[i][j] = 1 si el curso i es prerrequisito del curso j

    Nodo* raiz_arbol = nullptr;

    int siguiente_id_estudiante = 1;
    int siguiente_id_docente    = 1;
    int siguiente_id_curso      = 1;

    std::mutex mtx;                 // bloquear en cada handler que lea o modifique el estado
};

void inicializar_matrices(Estado& e);                       // notas = SIN_NOTA, asistencia = SIN_REGISTRO, adyacencia = 0
void quitar_estudiante_en(Estado& e, size_t indice);        // elimina al estudiante Y desplaza las filas de notas y asistencia
int  indice_estudiante_por_id(const Estado& e, int id);     // -1 si no existe
int  indice_curso_por_id(const Estado& e, int id);          // -1 si no existe
```

Implementa esas tres funciones en `estado.cpp`. `quitar_estudiante_en` es **importante**: al eliminar un estudiante deben desplazarse las filas de ambas matrices para que no queden desalineadas.

**Helpers de respuesta (`respuestas.hpp`)**

```cpp
void responder_ok(httplib::Response& res, const nlohmann::json& datos, int estado_http = 200);
void responder_error(httplib::Response& res, const std::string& mensaje, int estado_http = 400);
```

---

## 5. FASE 1 — Infraestructura (debe compilar y arrancar)

1. Crea la estructura de carpetas de la sección 3.
2. Descarga `httplib.h` y `json.hpp` a `backend/third_party/` (ver sección 4).
3. Crea `estado.hpp/.cpp` y `respuestas.hpp/.cpp` como se describe arriba.
4. Crea `main.cpp` con este comportamiento:
   - Crea el `Estado`, llama a `inicializar_matrices` y luego a `cargar_todo` (persistencia). Si no hay datos en `data/`, arranca vacío.
   - Configura CORS permisivo con `set_default_headers` y responde a `OPTIONS`.
   - Monta el frontend: `svr.set_mount_point("/", "./frontend");`
   - Registra `GET /api/salud` → `{ "ok": true, "datos": { "servicio": "gestion-escolar", "version": "0.1.0" } }`
   - Llama a las 7 funciones `registrar_rutas_*` de los módulos.
   - Maneja errores: `set_exception_handler` devuelve `500` con el formato estándar.
   - Escribe en consola la URL: `Servidor listo en http://localhost:<puerto>`.
5. Crea `CMakeLists.txt` y `Makefile`:
   - `make` compila a `build/gestion_escolar`; `make run` compila y ejecuta; `make clean` borra `build/`.
   - Toma todos los `backend/src/*.cpp` automáticamente (wildcard / `file(GLOB ...)`).
   - Detecta Windows (`ifeq ($(OS),Windows_NT)` / `if(WIN32)`) para enlazar `ws2_32 wsock32` y definir `_WIN32_WINNT=0x0A00`.

**Criterio de listo de la Fase 1:** `make` compila sin errores y `curl http://localhost:8080/api/salud` responde el JSON esperado.

---

## 6. FASE 2 — Módulos del backend (stubs que compilan)

Para **cada módulo** crea `.hpp` + `.cpp` con la función `registrar_rutas_<modulo>`. Los `GET` deben devolver datos reales leídos del `Estado` (cargado desde los JSON semilla). Las operaciones de escritura (`POST`/`PUT`/`DELETE`) pueden quedar como **versión mínima funcional** o con `responder_error(res, "No implementado todavía", 501)` más su `TODO(Pn, Txx)`.

### 6.1 `estudiantes` — DUEÑO P1 (T10)
- `GET    /api/estudiantes?buscar=&orden=nombre|id` → lista (filtra por nombre si hay `buscar`)
- `GET    /api/estudiantes/:id`
- `POST   /api/estudiantes` `{nombre, edad, grado, seccion}` → 201
- `PUT    /api/estudiantes/:id`
- `DELETE /api/estudiantes/:id` (debe usar `quitar_estudiante_en`)
- Deja `GET` funcionando; el resto con `TODO(P1, T10)`.

### 6.2 `docentes_cursos` — DUEÑO P5 (T14)
- `GET/POST /api/docentes` · `DELETE /api/docentes/:id`
- `GET/POST /api/cursos` · `DELETE /api/cursos/:id` (máximo `MAX_CURSOS`)
- `POST /api/asignaciones` `{docente_id, curso_id}` → asigna docente a curso
- Deja los `GET` funcionando; el resto con `TODO(P5, T14)`.

### 6.3 `notas` — DUEÑO P2 (T11, T15, T18)
- `GET /api/notas` → `{ "estudiantes": [...nombres], "cursos": [...nombres], "matriz": [[...]] }` (con `null` donde haya `SIN_NOTA`)
- `PUT /api/notas` `{estudiante_id, curso_id, nota}` (validar 0–100)
- `GET /api/estadisticas` → `{ "promedio_por_estudiante": [...], "promedio_por_curso": [...], "mayor_promedio": {...}, "menor_promedio": {...}, "aprobados": n, "reprobados": n, "promedio_general": x }`
- Deja `GET /api/notas` funcionando; `PUT` y `/api/estadisticas` con `TODO(P2, T11/T15/T18)`. **Ignorar `SIN_NOTA` en los promedios** (anótalo como comentario).

### 6.4 `asistencia` — DUEÑO P4 (T17)
- `GET /api/asistencia` → `{ "estudiantes": [...], "dias": ["L","M","X","J","V"], "matriz": [[...]], "porcentajes": [...] }`
- `PUT /api/asistencia` `{estudiante_id, dia, presente}` (`dia` de 0 a 4)
- `GET` funcionando; `PUT` con `TODO(P4, T17)`.

### 6.5 `reportes` — DUEÑO P4 (T25)
- `GET /api/reportes/ranking` → estudiantes ordenados por promedio (descendente)
- `GET /api/reportes/asistencia` → ranking de asistencia
- `GET /api/reportes/cursos-relacionados?curso=ID` → usa BFS del módulo `grafo`
- Los tres con `TODO(P4, T25)`; pueden devolver listas vacías.

### 6.6 `arbol` — DUEÑO P3 (T12)
- `GET  /api/arbol` → árbol anidado `{ "nombre": "Colegio", "hijos": [ ... ] }`
- `GET  /api/arbol/recorrido?tipo=preorden|postorden|niveles` → lista de nombres
- `POST /api/arbol/nodo` `{padre, nombre}`
- Construye el árbol semilla en memoria (sección 8). `GET /api/arbol` funcionando; recorrido y `POST` con `TODO(P3, T12)`.

### 6.7 `grafo` — DUEÑO P3 (T21, T26)
- `GET  /api/grafo` → `{ "vertices": [{id, nombre}], "aristas": [{desde, hasta}], "matriz": [[...]] }`
- `POST /api/grafo/arista` `{desde, hasta}`
- `GET  /api/grafo/bfs?inicio=ID` · `GET /api/grafo/dfs?inicio=ID` → orden de visita
- `GET  /api/grafo/ruta?desde=ID&hasta=ID` → camino de prerrequisitos
- `GET /api/grafo` funcionando; el resto con `TODO(P3, T21/T26)`.

### 6.8 `persistencia` — DUEÑO P2 (T24)
- `bool guardar_todo(Estado& e, const std::string& dir = "data");`
- `bool cargar_todo(Estado& e, const std::string& dir = "data");`
- `POST /api/guardar` y `POST /api/cargar`
- Entrega una versión **básica funcional** (leer y escribir los JSON de `data/`). P2 la refina después (copias en `data/backup/`, validaciones, manejo de archivos corruptos con `TODO(P2, T24)`).

**Criterio de listo de la Fase 2:** compila sin warnings graves y todos los `GET` devuelven datos de la semilla.

---

## 7. FASE 3 — Frontend base

**`frontend/index.html` (shell)**
- Barra lateral con un enlace por vista: Estudiantes, Docentes y Cursos, Calificaciones, Asistencia, Reportes, Estructura académica, Prerrequisitos.
- Cada enlace muestra un puntito del color de su dueño (sección 2) — es el mapa visual del equipo.
- Un contenedor `<main id="contenido">` donde se carga la vista activa y un contenedor `<div id="mensajes">` para avisos.
- Carga `css/estilos.css`, `js/api.js` y `js/app.js`.

**`frontend/js/app.js` (router por hash)**
- Rutas: `#/estudiantes`, `#/docentes-cursos`, `#/calificaciones`, `#/asistencia`, `#/reportes`, `#/arbol`, `#/grafo`. Por defecto `#/estudiantes`.
- Al cambiar de ruta: hace `fetch` de `vistas/vista-<nombre>.html`, lo inyecta en `#contenido`, carga dinámicamente `vistas/vista-<nombre>.js` y llama a `window.Vistas["<nombre>"].iniciar()`.
- Marca como activo el enlace actual del menú.

**Contrato de cada vista JS** (todas lo cumplen):

```js
window.Vistas = window.Vistas || {};
window.Vistas["estudiantes"] = {
  async iniciar() {
    // TODO(P1, T13): reemplazar por la vista real
    const datos = await API.get("/api/estudiantes");
    document.getElementById("tabla-estudiantes").innerHTML =
      tabla(["ID", "Nombre", "Edad", "Grado", "Sección"],
            datos.map(e => [e.id, e.nombre, e.edad, e.grado, e.seccion]));
  }
};
```

**`frontend/js/api.js` (utilidades compartidas)**
- `API.get(ruta)`, `API.post(ruta, cuerpo)`, `API.put(ruta, cuerpo)`, `API.del(ruta)`: devuelven `datos` si `ok` es `true`; si no, lanzan `Error(error)` y muestran el aviso.
- `mostrarMensaje(texto, tipo)` con `tipo` = `"ok" | "error" | "info"` (aviso que desaparece solo a los 3 s).
- `esc(texto)` para escapar HTML.
- `tabla(encabezados, filas)` devuelve el HTML de una tabla ya escapado.
- `colorNota(nota)` devuelve una clase CSS (`nota-baja`, `nota-media`, `nota-alta`, `nota-vacia`) para la matriz de calificaciones.

**`frontend/css/estilos.css`**
- Diseño limpio y claro: paleta azul marino `#1F3A5F` + verde azulado `#1A9E9A`, tipografía sans-serif del sistema, tarjetas con sombra suave, tablas con filas alternas, botones, formularios, avisos, y clases de colores de nota. Variables CSS para los colores de P1–P5. Que se vea bien en pantallas de 360 px en adelante.

**Vistas plantilla (7 pares `.html` + `.js`)**
- Cada `.html` tiene un título (`<h1>`), una frase indicando el **dueño** y las tareas, y un contenedor vacío donde la vista pintará sus datos (por ejemplo `<div id="tabla-estudiantes"></div>`).
- Cada `.js` cumple el contrato de arriba y **ya muestra datos reales** de su `GET` principal en una tabla básica (para que todos tengan un ejemplo funcional de `fetch` desde el día 1). Debe llevar `TODO(Pn, Txx)` donde el dueño construirá la vista final.
- `vista-arbol.js` puede mostrar el JSON del árbol como lista anidada simple; `vista-grafo.js` la matriz de adyacencia como tabla.

**Criterio de listo de la Fase 3:** al abrir `http://localhost:8080` se ve el menú, se puede navegar entre las 7 vistas y cada una muestra sus datos de la semilla.

---

## 8. FASE 4 — Datos semilla (`data/`)

Usa nombres y datos realistas y consistentes entre archivos.

- **Estudiantes (6):** Diego, Carlos, Ana, María, Luis, Sofía (con edad, grado y sección: p. ej. "1ro Básico" A/B, "4to Diversificado" A).
- **Docentes (3):** con su especialidad (Matemática, Programación, Bases de Datos).
- **Cursos (8), en este orden (índices 0–7):** Matemática I, Matemática II, Álgebra Lineal, Cálculo, Programación I, Programación II, Estructuras de Datos, Bases de Datos. Asigna docentes a algunos cursos.
- **Notas:** matriz 6×8 con valores entre 55 y 98 y algunos `null` (sin nota), de modo que haya aprobados y reprobados.
- **Asistencia:** matriz 6×5 con 1 y 0 variados.
- **Grafo (aristas = prerrequisito → curso):** Matemática I→Matemática II · Matemática II→Álgebra Lineal · Álgebra Lineal→Cálculo · Programación I→Programación II · Programación I→Estructuras de Datos · Programación II→Estructuras de Datos · Estructuras de Datos→Bases de Datos.
- **Árbol:** Colegio → Básicos (1ro, 2do, 3ro, cada uno con secciones A y B) y Diversificado (4to, 5to).

---

## 9. FASE 5 — Documentación

Crea estos archivos con contenido útil (no plantillas vacías):

1. **`README.md`** — qué es el proyecto, cómo compilar y ejecutar en Linux/macOS/Windows (MinGW), cómo abrir el frontend, estructura de carpetas y a quién preguntar qué (tabla de dueños). Incluye el requisito: `g++` con soporte C++17.
2. **`CLAUDE.md`** — contexto permanente para futuras sesiones de Claude Code: resumen del proyecto, stack, convenciones (secciones 0 y 4 de este archivo), regla de "un archivo por módulo", formato de marcadores `TODO(Pn, Txx)`, comandos de compilar/ejecutar/probar y la tabla de dueños.
3. **`docs/API.md`** — todos los endpoints de la sección 6 con método, ruta, cuerpo de ejemplo y respuesta de ejemplo en JSON.
4. **`docs/REPARTO.md`** — reparto por persona con colores, vistas, archivos y tareas (sección 2).
5. **`docs/GIT.md`** — flujo de trabajo:
   - Una rama por persona: `p1-estudiantes`, `p2-notas`, `p3-arbol-grafo`, `p4-shell-asistencia`, `p5-docentes-qa`.
   - Commits pequeños con mensajes en español (`feat(notas): ...`, `fix(grafo): ...`).
   - **Merge diario a `main`** antes de cerrar el día (rutina de 20 min); si algo se rompe, se arregla en el momento.
   - Nadie edita el archivo de otra persona sin avisar; `main.cpp` solo registra rutas.
   - Cómo resolver conflictos y cómo actualizar tu rama con `git pull --rebase origin main`.
6. **`docs/ARQUITECTURA.md`** — diagrama en ASCII del sistema y explicación de dónde se usa cada estructura (tabla de la sección 1), incluyendo la convención "fila = posición del estudiante, columna = posición del curso".
7. **`docs/DECISIONES.md`** — decisiones que tomaste al generar la base y cualquier paso que no pudiste completar.
8. **`tests/smoke.sh`** — script bash que, con el servidor corriendo, hace `curl` a `/api/salud` y a todos los `GET` principales y verifica que la respuesta contenga `"ok":true`; termina con código distinto de 0 si algo falla.
9. **`tests/casos_de_prueba.md`** — plantilla en tabla (ID, módulo, caso, datos de entrada, resultado esperado, resultado obtenido, estado) con 2 casos de ejemplo por módulo.
10. **`.gitignore`** — `build/`, `*.o`, `*.exe`, `.vscode/`, `.idea/`, `.DS_Store`, `data/backup/*` (pero conserva `data/backup/.gitkeep`).

---

## 10. FASE 6 — Git (solo local)

1. Si no existe repo: `git init` y usa la rama `main`.
2. Haz un commit inicial: `chore: base del proyecto (andamiaje de Operación Pizarrón)`.
3. Crea las 5 ramas locales de `docs/GIT.md` a partir de `main` y **vuelve a `main`**.
4. **No** hagas `push` ni configures remotos.

---

## 11. Verificación obligatoria (antes de terminar)

Ejecuta y corrige hasta que todo pase:

1. `make` compila sin errores.
2. Arranca el servidor en segundo plano, ejecuta `bash tests/smoke.sh` y apágalo.
3. `curl -s http://localhost:8080/` devuelve el `index.html` del frontend.
4. `grep -rn "TODO(P" backend frontend` lista los marcadores y **cada módulo y cada vista** tiene al menos uno.
5. Todos los archivos de la sección 3 existen y ninguno de los `.json` de `data/` está vacío ni es inválido (`python3 -m json.tool` o equivalente).
6. `git status` está limpio y `git branch` muestra `main` y las 5 ramas.

Si algo falla y no puedes arreglarlo, no lo escondas: documéntalo en `docs/DECISIONES.md` y repórtalo en el resumen.

---

## 12. Resumen final que debes entregar

Al terminar, responde con:

- ✅ / ❌ de cada punto de la sección 11.
- Árbol de carpetas resultante (2 niveles).
- Comandos exactos para compilar, ejecutar y probar.
- Lista de cosas pendientes o decisiones importantes (de `docs/DECISIONES.md`).
- Los 3 primeros pasos que debe hacer cada persona del equipo el **Día 2**, según el plan (P1: endpoints de Estudiantes · P2: matriz de notas · P3: árbol académico · P4: shell y asistencia · P5: structs/vectores y Docentes y Cursos).

---

## 13. Lo que NO debes hacer

- No implementar toda la lógica final de los módulos (BFS, estadísticas, CRUD completo, etc.): eso es trabajo del equipo.
- No agregar frameworks de frontend (React, Vue, etc.) ni librerías de CSS externas: solo HTML, CSS y JavaScript vanilla.
- No cambiar las dimensiones ni los nombres de las estructuras de `estado.hpp` sin dejarlo anotado en `docs/DECISIONES.md`.
- No hacer commits de `build/`, binarios ni copias de seguridad de datos.
- No inventar el contenido de las librerías de terceros.
