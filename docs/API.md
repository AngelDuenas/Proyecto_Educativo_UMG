# API del backend — Operación Pizarrón

Servidor C++ (cpp-httplib + nlohmann/json). Por defecto escucha en `http://localhost:8080` y también sirve el frontend.

## Convenciones

- **Respuesta correcta:** el JSON de los datos directamente (sin envoltorio).
- **Error:** código HTTP 4xx/5xx y cuerpo `{"error": "mensaje para el usuario"}`.
- **Códigos usados:** `200` correcto · `201` creado · `400` datos inválidos · `404` no existe · `409` duplicado · `500` error interno.
- **Datos en memoria:** los cambios se pierden al apagar el servidor, salvo que se llame a `POST /api/guardar`.
- **Límites:** 100 estudiantes, 15 cursos, 5 días de asistencia (L, M, X, J, V).
- **Aprobación:** una nota de **61 o más** aprueba.

---

## Estudiantes — `estudiantes.cpp`

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/estudiantes?buscar=&orden=` | Lista. `buscar` filtra por nombre (sin distinguir tildes) o carnet. `orden`: `nombre` (por defecto), `nombre-desc`, `carnet`, `semestre`. |
| POST | `/api/estudiantes` | Registra. Cuerpo: `{carnet*, nombre*, carrera, semestre}`. `semestre` de 1 a 12 (por defecto 1). Responde `201`. |
| PUT | `/api/estudiantes/{carnet}` | Edita. Cuerpo parcial: `{nombre, carrera, semestre}`. El carnet no cambia. |
| DELETE | `/api/estudiantes/{carnet}` | Elimina. Responde `{"ok": true}`. |

Estudiante: `{"carnet": "0901-26-1001", "nombre": "Ana Lucía Pérez", "carrera": "Ingeniería en Sistemas", "semestre": 2}`

Errores: `400` carnet/nombre vacíos o semestre fuera de rango · `404` carnet inexistente · `409` carnet repetido.

## Notas — `notas.cpp`

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/notas` | Matriz completa. |
| PUT | `/api/notas` | Cuerpo: `{carnet, curso, nota}`. `nota` de 0 a 100, o `null` para borrarla. |
| GET | `/api/estadisticas` | Promedios y resumen. |

`GET /api/notas`:
```json
{
  "estudiantes": [{"carnet": "0901-26-1001", "nombre": "Ana Lucía Pérez"}],
  "cursos": [{"codigo": "MAT101", "nombre": "Matemática I"}],
  "matriz": [[85, null, 90]]
}
```
Cada fila es un estudiante y cada columna un curso, en el mismo orden de `estudiantes` y `cursos`. `null` = sin nota.

`GET /api/estadisticas`:
```json
{
  "promedioGeneral": 71.43,
  "porEstudiante": [{"carnet": "0901-26-1001", "promedio": 76.5}],
  "porCurso": [{"codigo": "MAT101", "promedio": 69.7}],
  "mayor": {"nota": 100, "carnet": "0901-26-1001", "curso": "LOG101"},
  "menor": {"nota": 43, "carnet": "0901-26-1006", "curso": "MAT102"},
  "aprobados": 55,
  "reprobados": 25
}
```
`aprobados` y `reprobados` cuentan **notas**, no estudiantes. Los promedios son `null` si no hay notas.

## Asistencia — `asistencia.cpp`

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/asistencia` | Matriz de la semana y porcentajes. |
| PUT | `/api/asistencia` | Cuerpo: `{carnet, dia, valor}`. `dia` de 0 (lunes) a 4 (viernes). `valor`: `1`/`true` presente, `0`/`false` ausente. |

`GET /api/asistencia`:
```json
{
  "dias": ["L", "M", "X", "J", "V"],
  "estudiantes": [{"carnet": "0901-26-1001", "nombre": "Ana Lucía Pérez"}],
  "matriz": [[1, 1, 0, 1, 1]],
  "porEstudiante": [80],
  "porDia": [100, 100, 0, 100, 100]
}
```
Porcentajes con un decimal.

## Reportes — `reportes.cpp`

`GET /api/reportes` → rankings ya ordenados de mayor a menor:
```json
{
  "rankingNotas": [{"carnet": "...", "nombre": "...", "promedio": 76.5}],
  "rankingAsistencia": [{"carnet": "...", "nombre": "...", "porcentaje": 100}]
}
```

## Grafo de prerrequisitos — `grafo.cpp`

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/grafo` | `{nodos: [{codigo, nombre}], matriz}`. `matriz[i][j] = 1` si el curso `i` es prerrequisito del curso `j`. |
| GET | `/api/grafo/bfs?origen=MAT101` | Recorrido en anchura: `{origen, orden: ["MAT101", "MAT102", ...]}` |
| GET | `/api/grafo/dfs?origen=MAT101` | Recorrido en profundidad, mismo formato. |

`404` si el curso de origen no existe.

## Docentes, cursos, asignaciones y árbol — `rutas_catalogo.cpp`

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/docentes` | Lista de docentes (módulo `docentes.cpp`). |
| POST | `/api/docentes` | `{nombre*, especialidad}` |
| DELETE | `/api/docentes/{id}` | También quita sus asignaciones. |
| GET | `/api/cursos` | `[{codigo, nombre, creditos, semestre}]`. El catálogo vive en `Estado`, igual que las columnas de la matriz de notas. |
| POST | `/api/cursos` | `{codigo*, nombre*, creditos, semestre}`. Agrega una columna a la matriz. `409` si el código existe. |
| DELETE | `/api/cursos/{codigo}` | Quita la columna de notas y sus prerrequisitos. |
| GET | `/api/asignaciones` | Lista de `{docenteId, cursoCodigo}`. |
| POST | `/api/asignaciones` | `{docenteId, cursoCodigo}`. `404` si no existen, `409` si ya está asignado. |
| DELETE | `/api/asignaciones/{docenteId}/{cursoCodigo}` | Elimina la asignación. |
| GET | `/api/arbol` | Árbol académico (módulo `arbol.cpp`). |

## Persistencia y estado — `persistencia.cpp`

| Método | Ruta | Descripción |
|---|---|---|
| POST | `/api/guardar` | Escribe `estudiantes.json`, `cursos.json`, `notas.json` y `asistencia.json` en `data/`. Responde `{"guardado": true}`. |
| POST | `/api/cargar` | Lee esos archivos y reemplaza los datos en memoria. `500` si faltan o son inválidos. |
| GET | `/api/salud` | `{"servicio": "gestion-escolar", "version": "0.2.0"}` |

Al arrancar, el servidor carga datos de ejemplo y, si los cuatro archivos de `data/` existen y son válidos, los usa en su lugar.

---

## Cómo está organizado el código

| Archivo | Qué contiene |
|---|---|
| `estado.hpp/.cpp` | Estructuras (`Estudiante`, `CursoEstado`, `Estado`), matrices de notas, asistencia y adyacencia, y las operaciones de agregar/quitar con desplazamiento de filas y columnas. |
| `algoritmos.hpp/.cpp` | Promedios por fila y columna, extremos, aprobados, porcentajes, rankings (insertion sort estable), BFS y DFS. Sin librerías externas. |
| `respuestas.hpp/.cpp` | Respuestas JSON y errores. |
| `estudiantes.cpp`, `notas.cpp`, `asistencia.cpp`, `reportes.cpp`, `grafo.cpp` | Rutas HTTP de cada módulo. |
| `rutas_catalogo.cpp` | Rutas que conectan `docentes.cpp` y `arbol.cpp` con el servidor. |
| `persistencia.cpp` | Guardar y cargar en `data/`. |
| `main.cpp` | Crea el servidor y registra las rutas. |

## Cómo compilar y ejecutar

```
mingw32-make -j4     # compila a build/gestion_escolar.exe
mingw32-make run     # arranca el servidor en http://localhost:8080
```
