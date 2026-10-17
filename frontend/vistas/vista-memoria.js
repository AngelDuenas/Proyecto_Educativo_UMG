/* =====================================================================
   Vista Matrices en memoria (solo lectura)
   ---------------------------------------------------------------------
   GET /api/memoria →
     { capacidad: {estudiantes, cursos, dias, periodos},
       estudiantes: [{fila, carnet, nombre}], cursos: [{columna, codigo, nombre}],
       notas: (n+1)×m crudos (-1 = sin nota; la última fila es la siguiente libre),
       asistencia: (n+1)×5, adyacencia: m×m, horario: 5×6 listas de códigos,
       ocupacion: {filasUsadas, columnasUsadas, celdasTotales, notasRegistradas} }
   ===================================================================== */

const contenedor = document.getElementById("contenido-memoria");
const cifrasEl = document.getElementById("cifras");
const casillaAuto = document.getElementById("auto");
const DIAS = ["L", "M", "X", "J", "V"];

let anterior = null;   // última lectura, para iluminar las celdas que cambiaron

// Tabla de una matriz cruda: encabezados [i] / [j] y la celda iluminada si cambió.
function tablaCruda(id, filas, columnas, valores, { clase = () => "", libre = false } = {}) {
  const viejo = anterior && anterior[id];
  const cab = columnas.map((c) => `<th class="text-center">${c}</th>`).join("");
  const cuerpo = valores
    .map((fila, i) => {
      const esLibre = libre && i === valores.length - 1;
      const celdas = fila
        .map((v, j) => {
          const cambio = viejo && viejo[i] && viejo[i][j] !== undefined && viejo[i][j] !== v ? " cambio" : "";
          return `<td class="celda ${clase(v)}${cambio}" title="${id}[${i}][${j}]">${esc(v)}</td>`;
        })
        .join("");
      return `<tr class="${esLibre ? "fila-libre" : ""}"><th>${filas[i]}</th>${celdas}</tr>`;
    })
    .join("");
  return `<div class="tabla-contenedor"><table class="tabla matriz"><thead><tr><th></th>${cab}</tr></thead><tbody>${cuerpo}</tbody></table></div>`;
}

function seccion(titulo, declaracion, explicacion, cuerpo) {
  return `<section class="panel mb-5">
    <div class="panel-encabezado"><div><h2>${titulo}</h2><p>${explicacion}</p></div></div>
    <div class="formula">${declaracion}</div>
    ${cuerpo}
  </section>`;
}

function dibujar(d) {
  const { capacidad: cap, ocupacion: oc } = d;
  const n = d.estudiantes.length;
  const filas = d.notas.map((_, i) =>
    i < n ? `[${i}] <span class="sub">${esc(d.estudiantes[i].carnet)}</span>` : `[${i}] <span class="sub">libre</span>`
  );
  const colCursos = d.cursos.map((c) => `[${c.columna}]<span class="sub">${esc(c.codigo)}</span>`);
  const hayLibre = d.notas.length > n;

  cifrasEl.innerHTML = `
    <div class="cifra info"><span class="cifra-texto">Filas usadas</span><span class="cifra-valor">${oc.filasUsadas} / ${cap.estudiantes}</span><span class="cifra-detalle">estudiantes</span></div>
    <div class="cifra info"><span class="cifra-texto">Columnas usadas</span><span class="cifra-valor">${oc.columnasUsadas} / ${cap.cursos}</span><span class="cifra-detalle">cursos</span></div>
    <div class="cifra ok"><span class="cifra-texto">Notas registradas</span><span class="cifra-valor">${oc.notasRegistradas}</span><span class="cifra-detalle">celdas distintas de -1</span></div>
    <div class="cifra"><span class="cifra-texto">Celdas reservadas</span><span class="cifra-valor">${formatear(oc.celdasTotales, 0)}</span><span class="cifra-detalle">float notas[${cap.estudiantes}][${cap.cursos}]</span></div>`;

  const notas = seccion(
    "Matriz de notas",
    `float notas[${cap.estudiantes}][${cap.cursos}]; &nbsp;·&nbsp; fila = estudiante, columna = curso`,
    `La fila <i>i</i> es la posición del estudiante en la lista y la columna <i>j</i> la del curso. Al guardar una nota se escribe en <code>notas[i][j]</code>; <code>-1</code> significa «sin nota».${hayLibre ? " La última fila (gris) es la que ocupará el próximo estudiante." : ""}`,
    tablaCruda("notas", filas, colCursos, d.notas, { clase: (v) => (v < 0 ? "nota-vacia" : claseNota(v)), libre: hayLibre })
  );

  const asistencia = seccion(
    "Matriz de asistencia",
    `int asistencia[${cap.estudiantes}][${cap.dias}]; &nbsp;·&nbsp; 1 = presente, 0 = ausente`,
    "Misma fila que en la matriz de notas; cada columna es un día de la semana.",
    tablaCruda("asistencia", filas, DIAS.map((x, i) => `[${i}]<span class="sub">${x}</span>`), d.asistencia, {
      clase: (v) => (v ? "" : "nota-baja"),
      libre: hayLibre,
    })
  );

  const codigos = d.cursos.map((c) => c.codigo);
  const adyacencia = seccion(
    "Matriz de adyacencia (prerrequisitos)",
    `int adyacencia[${cap.cursos}][${cap.cursos}]; &nbsp;·&nbsp; [i][j] = 1 si el curso i es prerrequisito de j`,
    "Es el grafo de la vista Prerrequisitos guardado como matriz.",
    tablaCruda("adyacencia", codigos.map((c, i) => `[${i}] <span class="sub">${esc(c)}</span>`), colCursos, d.adyacencia, {
      clase: (v) => (v ? "uno" : "cero"),
    })
  );

  const horarioTexto = d.horario.map((dia) => dia.map((bloque) => (bloque.length ? bloque.join(", ") : "·")));
  const horario = seccion(
    "Matriz del horario",
    `vector&lt;string&gt; horario[${cap.dias}][${cap.periodos}]; &nbsp;·&nbsp; fila = día, columna = periodo`,
    "Cada celda guarda la lista de cursos que se dan en ese bloque.",
    tablaCruda("horario", DIAS.map((x, i) => `[${i}] <span class="sub">${x}</span>`), [...Array(cap.periodos)].map((_, p) => `[${p}]`), horarioTexto)
  );

  // Se conserva el desplazamiento lateral de cada tabla al redibujar.
  const desplazamientos = [...contenedor.querySelectorAll(".tabla-contenedor")].map((t) => t.scrollLeft);
  contenedor.innerHTML = notas + asistencia + adyacencia + horario;
  contenedor.querySelectorAll(".tabla-contenedor").forEach((t, i) => (t.scrollLeft = desplazamientos[i] || 0));
  contenedor.querySelectorAll("table").forEach((t, i) => i === 2 && t.classList.add("adyacencia"));
  anterior = { notas: d.notas, asistencia: d.asistencia, adyacencia: d.adyacencia, horario: horarioTexto };
}

async function cargar() {
  try {
    dibujar(await api("/memoria"));
  } catch (e) {
    contenedor.innerHTML = htmlError(`No se pudo leer la memoria del servidor: ${e.message}`);
  }
}

// Se vuelve a leer cada 2 s (solo si la pestaña está visible y la casilla marcada).
setInterval(() => {
  if (casillaAuto.checked && !document.hidden) cargar();
}, 2000);

cargar();
