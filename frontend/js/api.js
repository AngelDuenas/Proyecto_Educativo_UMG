/* =====================================================================
   api.js · Conexión entre las vistas y el servidor C++
   ---------------------------------------------------------------------
   Todas las vistas llaman a:   api("/estudiantes", "POST", datos)

   - Si el servidor C++ responde, se usan sus datos reales.
   - Si NO responde (no está corriendo, o abrieron el HTML con doble
     clic), se activa el MODO DEMO: un servidor falso en memoria con
     datos de ejemplo. Así cada integrante puede trabajar su vista aunque
     el backend todavía no esté listo. El formato JSON del modo demo es
     el mismo que debe devolver el C++ (ver docs al inicio de cada vista).
   ===================================================================== */

// Si la página la sirve el propio servidor C++ (puerto 8080) se usa ruta
// relativa. Si la abren con Live Server u otro puerto, apunta a localhost.
const API_BASE = location.port === "8080" ? "/api" : "http://localhost:8080/api";

let modoDemo = false;

class ApiError extends Error {
  constructor(estado, mensaje) {
    super(mensaje);
    this.estado = estado;
  }
}

async function api(ruta, metodo = "GET", datos) {
  if (!modoDemo) {
    let respuesta;
    try {
      respuesta = await fetch(API_BASE + ruta, {
        method: metodo,
        headers: datos ? { "Content-Type": "application/json" } : {},
        body: datos ? JSON.stringify(datos) : undefined,
      });
    } catch (e) {
      // TypeError = no hay servidor (o CORS bloqueado): pasamos a demo
      activarModoDemo();
      return servidorDemo(ruta, metodo, datos);
    }

    avisarEstado("conectado");
    const texto = await respuesta.text();
    let cuerpo = null;
    try {
      cuerpo = texto ? JSON.parse(texto) : null;
    } catch {
      cuerpo = texto;
    }

    if (!respuesta.ok) {
      const mensaje = (cuerpo && cuerpo.error) || texto || `Error ${respuesta.status}`;
      throw new ApiError(respuesta.status, mensaje);
    }
    return cuerpo;
  }
  return servidorDemo(ruta, metodo, datos);
}

function activarModoDemo() {
  if (modoDemo) return;
  modoDemo = true;
  avisarEstado("demo");
}

function avisarEstado(estado) {
  if (window.estadoServidor === estado) return;
  window.estadoServidor = estado;
  document.dispatchEvent(new CustomEvent("servidor-estado", { detail: estado }));
}

/* =====================================================================
   SERVIDOR DEMO (solo para desarrollo, el C++ debe replicar esto)
   ===================================================================== */

const CLAVE_DEMO = "pizarron-demo-v1";

function crearDatosDemo() {
  const estudiantes = [
    { carnet: "0901-26-1001", nombre: "Ana Lucía Pérez", carrera: "Ingeniería en Sistemas", semestre: 2 },
    { carnet: "0901-26-1002", nombre: "Bryan Estuardo López", carrera: "Ingeniería en Sistemas", semestre: 2 },
    { carnet: "0901-26-1003", nombre: "Carla Sofía Méndez", carrera: "Ingeniería en Sistemas", semestre: 2 },
    { carnet: "0901-26-1004", nombre: "Diego Alejandro Ramírez", carrera: "Ingeniería en Sistemas", semestre: 2 },
    { carnet: "0901-26-1005", nombre: "Elena María Castillo", carrera: "Ingeniería en Sistemas", semestre: 2 },
    { carnet: "0901-26-1006", nombre: "Fernando José Ajú", carrera: "Ingeniería en Sistemas", semestre: 2 },
    { carnet: "0901-26-1007", nombre: "Gabriela Andrea Xol", carrera: "Ingeniería en Sistemas", semestre: 2 },
    { carnet: "0901-26-1008", nombre: "Héctor Manuel Ortiz", carrera: "Ingeniería en Sistemas", semestre: 2 },
  ];

  const cursos = [
    { codigo: "MAT101", nombre: "Matemática I", creditos: 5, semestre: 1 },
    { codigo: "PRG101", nombre: "Introducción a la Programación", creditos: 5, semestre: 1 },
    { codigo: "LOG101", nombre: "Lógica de Sistemas", creditos: 4, semestre: 1 },
    { codigo: "MAT102", nombre: "Matemática II", creditos: 5, semestre: 2 },
    { codigo: "PRG102", nombre: "Programación I", creditos: 5, semestre: 2 },
    { codigo: "FIS101", nombre: "Física I", creditos: 4, semestre: 2 },
    { codigo: "ALG201", nombre: "Álgebra Lineal", creditos: 5, semestre: 3 },
    { codigo: "MDS201", nombre: "Matemática Discreta", creditos: 5, semestre: 3 },
    { codigo: "PRG201", nombre: "Programación II", creditos: 5, semestre: 3 },
    { codigo: "EDD301", nombre: "Estructura de Datos", creditos: 5, semestre: 4 },
  ];

  const docentes = [
    { id: "D01", nombre: "Ing. Carlos Méndez", especialidad: "Matemática" },
    { id: "D02", nombre: "Inga. Lucía Herrera", especialidad: "Programación" },
    { id: "D03", nombre: "Ing. Roberto Sic", especialidad: "Física" },
    { id: "D04", nombre: "Inga. Marta Quiñónez", especialidad: "Ciencias de la computación" },
  ];

  const asignaciones = [
    { docenteId: "D01", cursoCodigo: "MAT101" },
    { docenteId: "D01", cursoCodigo: "MAT102" },
    { docenteId: "D02", cursoCodigo: "PRG101" },
    { docenteId: "D02", cursoCodigo: "PRG102" },
    { docenteId: "D03", cursoCodigo: "FIS101" },
    { docenteId: "D04", cursoCodigo: "MDS201" },
  ];

  // Notas deterministas entre 42 y 100 para que todos vean lo mismo
  const notas = {};
  estudiantes.forEach((e, i) => {
    notas[e.carnet] = {};
    cursos.forEach((c, j) => {
      notas[e.carnet][c.codigo] = 42 + ((i * 37 + j * 53 + 11) % 59);
    });
  });

  const asistencia = {};
  estudiantes.forEach((e, i) => {
    asistencia[e.carnet] = [0, 1, 2, 3, 4].map((d) => ((i * 3 + d * 7) % 9 === 0 ? 0 : 1));
  });

  // Arista [A, B] = A es prerrequisito de B
  const prerrequisitos = [
    ["MAT101", "MAT102"],
    ["MAT101", "FIS101"],
    ["MAT102", "ALG201"],
    ["LOG101", "MDS201"],
    ["MAT102", "MDS201"],
    ["PRG101", "PRG102"],
    ["PRG102", "PRG201"],
    ["MDS201", "EDD301"],
    ["PRG201", "EDD301"],
  ];

  return { estudiantes, cursos, docentes, asignaciones, notas, asistencia, prerrequisitos };
}

function leerDemo() {
  try {
    const guardado = sessionStorage.getItem(CLAVE_DEMO);
    if (guardado) return JSON.parse(guardado);
  } catch { /* sin almacenamiento: usar datos nuevos */ }
  return crearDatosDemo();
}

function guardarDemo(bd) {
  try {
    sessionStorage.setItem(CLAVE_DEMO, JSON.stringify(bd));
  } catch { /* ignorar */ }
}

function promedio(lista) {
  const validos = lista.filter((n) => typeof n === "number");
  if (!validos.length) return null;
  return Math.round((validos.reduce((a, b) => a + b, 0) / validos.length) * 100) / 100;
}

function demoMatrizNotas(bd) {
  return bd.estudiantes.map((e) =>
    bd.cursos.map((c) => {
      const n = bd.notas[e.carnet] && bd.notas[e.carnet][c.codigo];
      return typeof n === "number" ? n : null;
    })
  );
}

function demoEstadisticas(bd) {
  const matriz = demoMatrizNotas(bd);
  const aprobacion = 61;
  let mayor = null;
  let menor = null;
  let aprobados = 0;
  let reprobados = 0;

  matriz.forEach((fila, i) =>
    fila.forEach((n, j) => {
      if (n === null) return;
      const dato = { nota: n, carnet: bd.estudiantes[i].carnet, curso: bd.cursos[j].codigo };
      if (!mayor || n > mayor.nota) mayor = dato;
      if (!menor || n < menor.nota) menor = dato;
      n >= aprobacion ? aprobados++ : reprobados++;
    })
  );

  return {
    promedioGeneral: promedio(matriz.flat()),
    porEstudiante: bd.estudiantes.map((e, i) => ({ carnet: e.carnet, promedio: promedio(matriz[i]) })),
    porCurso: bd.cursos.map((c, j) => ({ codigo: c.codigo, promedio: promedio(matriz.map((f) => f[j])) })),
    mayor,
    menor,
    aprobados,
    reprobados,
  };
}

function demoAsistencia(bd) {
  const matriz = bd.estudiantes.map((e) => bd.asistencia[e.carnet] || [1, 1, 1, 1, 1]);
  const pct = (lista) => Math.round((lista.reduce((a, b) => a + b, 0) / lista.length) * 1000) / 10;
  return {
    dias: ["L", "M", "X", "J", "V"],
    estudiantes: bd.estudiantes.map(({ carnet, nombre }) => ({ carnet, nombre })),
    matriz,
    porEstudiante: matriz.map((f) => (f.length ? pct(f) : 0)),
    porDia: [0, 1, 2, 3, 4].map((d) => (matriz.length ? pct(matriz.map((f) => f[d])) : 0)),
  };
}

function demoGrafo(bd) {
  const nodos = bd.cursos.map(({ codigo, nombre }) => ({ codigo, nombre }));
  const indice = Object.fromEntries(nodos.map((n, i) => [n.codigo, i]));
  const matriz = nodos.map(() => nodos.map(() => 0));
  bd.prerrequisitos.forEach(([a, b]) => {
    if (a in indice && b in indice) matriz[indice[a]][indice[b]] = 1;
  });
  return { nodos, matriz, indice };
}

function demoRecorrido(bd, origen, tipo) {
  const { nodos, matriz, indice } = demoGrafo(bd);
  if (!(origen in indice)) throw new ApiError(404, `No existe el curso ${origen}`);
  const n = nodos.length;
  const visitado = new Array(n).fill(false);
  const orden = [];

  if (tipo === "bfs") {
    const cola = [indice[origen]];
    visitado[indice[origen]] = true;
    while (cola.length) {
      const actual = cola.shift();
      orden.push(nodos[actual].codigo);
      for (let j = 0; j < n; j++) {
        if (matriz[actual][j] && !visitado[j]) {
          visitado[j] = true;
          cola.push(j);
        }
      }
    }
  } else {
    const dfs = (i) => {
      visitado[i] = true;
      orden.push(nodos[i].codigo);
      for (let j = 0; j < n; j++) if (matriz[i][j] && !visitado[j]) dfs(j);
    };
    dfs(indice[origen]);
  }
  return { origen, orden };
}

function demoArbol(bd) {
  const semestres = [...new Set(bd.cursos.map((c) => c.semestre))].sort((a, b) => a - b);
  const raiz = {
    nombre: "Universidad Mariano Gálvez",
    tipo: "Universidad",
    hijos: [
      {
        nombre: "Facultad de Ingeniería",
        tipo: "Facultad",
        hijos: [
          {
            nombre: "Ingeniería en Sistemas",
            tipo: "Carrera",
            hijos: semestres.map((s) => ({
              nombre: `Semestre ${s}`,
              tipo: "Semestre",
              hijos: bd.cursos
                .filter((c) => c.semestre === s)
                .map((c) => ({ nombre: c.nombre, tipo: "Curso", codigo: c.codigo, hijos: [] })),
            })),
          },
        ],
      },
    ],
  };
  const recorrido = [];
  const preorden = (nodo) => {
    recorrido.push(nodo.nombre);
    nodo.hijos.forEach(preorden);
  };
  preorden(raiz);
  return { raiz, recorrido };
}

function textoNormal(t) {
  return String(t).normalize("NFD").replace(/[\u0300-\u036f]/g, "").toLowerCase();
}

async function servidorDemo(rutaCompleta, metodo, datos) {
  await new Promise((r) => setTimeout(r, 120)); // simula red
  const bd = leerDemo();
  const [ruta, consulta = ""] = rutaCompleta.split("?");
  const params = new URLSearchParams(consulta);
  const partes = ruta.split("/").filter(Boolean).map(decodeURIComponent);
  const [recurso, id, id2] = partes;
  const clave = `${metodo} /${recurso}${id ? "/:id" : ""}`;
  let resultado;

  switch (clave) {
    /* ----- Estudiantes ----- */
    case "GET /estudiantes": {
      let lista = [...bd.estudiantes];
      const buscar = params.get("buscar");
      if (buscar) {
        const q = textoNormal(buscar);
        lista = lista.filter((e) => textoNormal(e.nombre).includes(q) || e.carnet.includes(buscar.trim()));
      }
      const orden = params.get("orden") || "nombre";
      const comparar = {
        nombre: (a, b) => a.nombre.localeCompare(b.nombre, "es"),
        "nombre-desc": (a, b) => b.nombre.localeCompare(a.nombre, "es"),
        carnet: (a, b) => a.carnet.localeCompare(b.carnet),
        semestre: (a, b) => a.semestre - b.semestre || a.nombre.localeCompare(b.nombre, "es"),
      }[orden];
      if (comparar) lista.sort(comparar);
      resultado = lista;
      break;
    }
    case "POST /estudiantes": {
      if (!datos.carnet || !datos.nombre) throw new ApiError(400, "El carnet y el nombre son obligatorios");
      if (bd.estudiantes.some((e) => e.carnet === datos.carnet))
        throw new ApiError(409, `Ya existe un estudiante con el carnet ${datos.carnet}`);
      bd.estudiantes.push(datos);
      bd.notas[datos.carnet] = {};
      bd.asistencia[datos.carnet] = [1, 1, 1, 1, 1];
      resultado = datos;
      break;
    }
    case "PUT /estudiantes/:id": {
      const e = bd.estudiantes.find((x) => x.carnet === id);
      if (!e) throw new ApiError(404, `No existe el estudiante ${id}`);
      Object.assign(e, datos, { carnet: id });
      resultado = e;
      break;
    }
    case "DELETE /estudiantes/:id": {
      const antes = bd.estudiantes.length;
      bd.estudiantes = bd.estudiantes.filter((x) => x.carnet !== id);
      if (bd.estudiantes.length === antes) throw new ApiError(404, `No existe el estudiante ${id}`);
      delete bd.notas[id];
      delete bd.asistencia[id];
      resultado = { ok: true };
      break;
    }

    /* ----- Docentes ----- */
    case "GET /docentes":
      resultado = bd.docentes;
      break;
    case "POST /docentes": {
      if (!datos.nombre) throw new ApiError(400, "El nombre del docente es obligatorio");
      const siguiente = bd.docentes.reduce((m, d) => Math.max(m, parseInt(d.id.slice(1), 10) || 0), 0) + 1;
      const nuevo = { id: "D" + String(siguiente).padStart(2, "0"), nombre: datos.nombre, especialidad: datos.especialidad || "" };
      bd.docentes.push(nuevo);
      resultado = nuevo;
      break;
    }
    case "DELETE /docentes/:id":
      bd.docentes = bd.docentes.filter((d) => d.id !== id);
      bd.asignaciones = bd.asignaciones.filter((a) => a.docenteId !== id);
      resultado = { ok: true };
      break;

    /* ----- Cursos ----- */
    case "GET /cursos":
      resultado = bd.cursos;
      break;
    case "POST /cursos": {
      if (!datos.codigo || !datos.nombre) throw new ApiError(400, "El código y el nombre son obligatorios");
      if (bd.cursos.some((c) => c.codigo === datos.codigo))
        throw new ApiError(409, `Ya existe un curso con el código ${datos.codigo}`);
      bd.cursos.push(datos);
      resultado = datos;
      break;
    }
    case "DELETE /cursos/:id":
      bd.cursos = bd.cursos.filter((c) => c.codigo !== id);
      bd.asignaciones = bd.asignaciones.filter((a) => a.cursoCodigo !== id);
      bd.prerrequisitos = bd.prerrequisitos.filter(([a, b]) => a !== id && b !== id);
      resultado = { ok: true };
      break;

    /* ----- Asignaciones ----- */
    case "GET /asignaciones":
      resultado = bd.asignaciones;
      break;
    case "POST /asignaciones": {
      if (bd.asignaciones.some((a) => a.docenteId === datos.docenteId && a.cursoCodigo === datos.cursoCodigo))
        throw new ApiError(409, "Ese docente ya tiene asignado ese curso");
      bd.asignaciones.push({ docenteId: datos.docenteId, cursoCodigo: datos.cursoCodigo });
      resultado = datos;
      break;
    }
    case "DELETE /asignaciones/:id":
      bd.asignaciones = bd.asignaciones.filter((a) => !(a.docenteId === id && a.cursoCodigo === id2));
      resultado = { ok: true };
      break;

    /* ----- Notas y estadísticas ----- */
    case "GET /notas":
      resultado = {
        estudiantes: bd.estudiantes.map(({ carnet, nombre }) => ({ carnet, nombre })),
        cursos: bd.cursos.map(({ codigo, nombre }) => ({ codigo, nombre })),
        matriz: demoMatrizNotas(bd),
      };
      break;
    case "PUT /notas": {
      const n = datos.nota;
      if (n !== null && (typeof n !== "number" || n < 0 || n > 100))
        throw new ApiError(400, "La nota debe estar entre 0 y 100");
      bd.notas[datos.carnet] = bd.notas[datos.carnet] || {};
      if (n === null) delete bd.notas[datos.carnet][datos.curso];
      else bd.notas[datos.carnet][datos.curso] = n;
      resultado = datos;
      break;
    }
    case "GET /estadisticas":
      resultado = demoEstadisticas(bd);
      break;

    /* ----- Asistencia ----- */
    case "GET /asistencia":
      resultado = demoAsistencia(bd);
      break;
    case "PUT /asistencia": {
      const fila = bd.asistencia[datos.carnet] || [1, 1, 1, 1, 1];
      fila[datos.dia] = datos.valor ? 1 : 0;
      bd.asistencia[datos.carnet] = fila;
      resultado = datos;
      break;
    }

    /* ----- Reportes ----- */
    case "GET /reportes": {
      const est = demoEstadisticas(bd);
      const asis = demoAsistencia(bd);
      const nombre = (c) => (bd.estudiantes.find((e) => e.carnet === c) || {}).nombre || c;
      resultado = {
        rankingNotas: est.porEstudiante
          .filter((p) => p.promedio !== null)
          .map((p) => ({ carnet: p.carnet, nombre: nombre(p.carnet), promedio: p.promedio }))
          .sort((a, b) => b.promedio - a.promedio),
        rankingAsistencia: asis.estudiantes
          .map((e, i) => ({ carnet: e.carnet, nombre: e.nombre, porcentaje: asis.porEstudiante[i] }))
          .sort((a, b) => b.porcentaje - a.porcentaje),
      };
      break;
    }

    /* ----- Árbol y grafo ----- */
    case "GET /arbol":
      resultado = demoArbol(bd);
      break;
    case "GET /grafo":
      if (id === "bfs" || id === "dfs") {
        resultado = demoRecorrido(bd, params.get("origen"), id);
      } else {
        const { nodos, matriz } = demoGrafo(bd);
        resultado = { nodos, matriz };
      }
      break;
    case "GET /grafo/:id":
      resultado = demoRecorrido(bd, params.get("origen"), id);
      break;

    default:
      throw new ApiError(404, `La ruta ${metodo} /api${ruta} no existe en el modo demo`);
  }

  guardarDemo(bd);
  // Copia para que la vista no modifique la "base" por accidente
  return JSON.parse(JSON.stringify(resultado));
}

function reiniciarDemo() {
  try {
    sessionStorage.removeItem(CLAVE_DEMO);
  } catch { /* ignorar */ }
}