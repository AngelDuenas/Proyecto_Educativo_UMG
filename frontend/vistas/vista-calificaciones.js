/* =====================================================================
   Vista Calificaciones (P2)
   ---------------------------------------------------------------------
   GET /api/notas →
     { "estudiantes": [ { "carnet": "...", "nombre": "..." } ],   // filas
       "cursos":      [ { "codigo": "MAT101", "nombre": "..." } ], // columnas
       "matriz":      [ [78, 65, null, ...], ... ] }              // notas[i][j]
   PUT /api/notas   body: { "carnet": "...", "curso": "MAT101", "nota": 85 }
                    (nota: null para borrarla)
   GET /api/estadisticas →
     { "promedioGeneral": 74.3,
       "porEstudiante": [ { "carnet": "...", "promedio": 80.5 } ],  // promedio por fila
       "porCurso":      [ { "codigo": "MAT101", "promedio": 71.2 } ], // promedio por columna
       "mayor": { "nota": 99, "carnet": "...", "curso": "MAT101" },
       "menor": { "nota": 42, "carnet": "...", "curso": "FIS101" },
       "aprobados": 60, "reprobados": 20 }
   ===================================================================== */

const matrizEl = document.getElementById("matriz-notas");
const cifrasEl = document.getElementById("cifras");
const dimensionEl = document.getElementById("dimension");

let datos = null; // respuesta de /notas
let stats = null; // respuesta de /estadisticas

async function cargar() {
  try {
    [datos, stats] = await Promise.all([api("/notas"), api("/estadisticas")]);
    dibujarMatriz();
    dibujarCifras();
  } catch (e) {
    matrizEl.innerHTML = htmlError(`No se pudieron cargar las notas: ${e.message}`);
  }
}

async function recargarEstadisticas() {
  try {
    stats = await api("/estadisticas");
    dibujarCifras();
    dibujarPromedios();
  } catch (e) {
    mostrarError(e, "No se pudieron recalcular las estadísticas");
  }
}

const nombreEstudiante = (carnet) => (datos.estudiantes.find((e) => e.carnet === carnet) || {}).nombre || carnet;

function dibujarCifras() {
  if (!stats) return;
  const total = stats.aprobados + stats.reprobados;
  const pct = total ? (stats.aprobados / total) * 100 : 0;
  const detalle = (d) => (d ? `${esc(nombreEstudiante(d.carnet))} en ${esc(d.curso)}` : "Sin datos");

  cifrasEl.innerHTML = `
    <div class="cifra info">
      <span class="cifra-texto">Promedio general</span>
      <span class="cifra-valor">${formatear(stats.promedioGeneral)}</span>
      <span class="cifra-detalle">De todas las notas registradas</span>
    </div>
    <div class="cifra ok">
      <span class="cifra-texto">Nota más alta</span>
      <span class="cifra-valor">${stats.mayor ? stats.mayor.nota : "—"}</span>
      <span class="cifra-detalle">${detalle(stats.mayor)}</span>
    </div>
    <div class="cifra mal">
      <span class="cifra-texto">Nota más baja</span>
      <span class="cifra-valor">${stats.menor ? stats.menor.nota : "—"}</span>
      <span class="cifra-detalle">${detalle(stats.menor)}</span>
    </div>
    <div class="cifra alerta">
      <span class="cifra-texto">Aprobados y reprobados</span>
      <span class="cifra-valor">${stats.aprobados} / ${stats.reprobados}</span>
      <span class="cifra-detalle">${formatear(pct, 0)} % de las notas aprueba (mínimo ${CONFIG.NOTA_APROBACION})</span>
    </div>`;
}

function dibujarMatriz() {
  const { estudiantes, cursos, matriz } = datos;
  dimensionEl.textContent = `${estudiantes.length} × ${cursos.length} (estudiantes × cursos)`;

  if (!estudiantes.length || !cursos.length) {
    matrizEl.innerHTML = htmlVacio(
      "La matriz está vacía.",
      " Registra estudiantes y cursos para empezar a ingresar notas."
    );
    return;
  }

  const encabezado = cursos
    .map((c) => `<th class="celda" title="${esc(c.nombre)}">${esc(c.codigo)}</th>`)
    .join("");

  const filas = estudiantes
    .map((e, i) => {
      const celdas = cursos
        .map((c, j) => {
          const n = matriz[i][j];
          return `<td class="celda">
            <input class="input-nota ${claseNota(n)}" type="number" min="0" max="100" step="1"
              value="${n ?? ""}" data-i="${i}" data-j="${j}"
              aria-label="Nota de ${esc(e.nombre)} en ${esc(c.nombre)}"
              title="notas[${i}][${j}]">
          </td>`;
        })
        .join("");
      return `<tr>
        <td>${esc(e.nombre)}<span class="sub">${esc(e.carnet)}</span></td>
        ${celdas}
        <td class="celda" data-promedio-fila="${i}">—</td>
      </tr>`;
    })
    .join("");

  const pie = cursos.map((_, j) => `<td class="celda" data-promedio-col="${j}">—</td>`).join("");

  matrizEl.innerHTML = `
    <div class="tabla-contenedor">
      <table class="tabla matriz">
        <thead><tr><th>Estudiante</th>${encabezado}<th class="celda">Promedio</th></tr></thead>
        <tbody>${filas}</tbody>
        <tfoot><tr><td>Promedio del curso</td>${pie}<td class="celda" id="promedio-general">—</td></tr></tfoot>
      </table>
    </div>`;

  dibujarPromedios();
}

function dibujarPromedios() {
  if (!stats || !datos) return;
  const pintar = (celda, valor) => {
    if (!celda) return;
    celda.textContent = formatear(valor);
    celda.className = `celda ${valor === null ? "" : claseNota(valor)}`;
  };
  datos.estudiantes.forEach((e, i) => {
    const p = stats.porEstudiante.find((x) => x.carnet === e.carnet);
    pintar(matrizEl.querySelector(`[data-promedio-fila="${i}"]`), p ? p.promedio : null);
  });
  datos.cursos.forEach((c, j) => {
    const p = stats.porCurso.find((x) => x.codigo === c.codigo);
    pintar(matrizEl.querySelector(`[data-promedio-col="${j}"]`), p ? p.promedio : null);
  });
  pintar(document.getElementById("promedio-general"), stats.promedioGeneral);
}

/* ---------- Edición de una celda ---------- */
matrizEl.addEventListener("change", async (ev) => {
  const input = ev.target.closest(".input-nota");
  if (!input) return;
  const i = Number(input.dataset.i);
  const j = Number(input.dataset.j);
  const anterior = datos.matriz[i][j];
  const texto = input.value.trim();
  const nota = texto === "" ? null : Number(texto);

  if (nota !== null && (!Number.isFinite(nota) || nota < 0 || nota > 100)) {
    input.setAttribute("aria-invalid", "true");
    toast("La nota debe estar entre 0 y 100.", "error");
    input.value = anterior ?? "";
    input.removeAttribute("aria-invalid");
    return;
  }

  try {
    await api("/notas", "PUT", {
      carnet: datos.estudiantes[i].carnet,
      curso: datos.cursos[j].codigo,
      nota,
    });
    datos.matriz[i][j] = nota;
    input.className = `input-nota ${claseNota(nota)}`;
    await recargarEstadisticas();
  } catch (e) {
    input.value = anterior ?? "";
    mostrarError(e, "No se guardó la nota");
  }
});

// Enter pasa a la celda de abajo, como en una hoja de cálculo
matrizEl.addEventListener("keydown", (ev) => {
  if (ev.key !== "Enter" || !ev.target.classList.contains("input-nota")) return;
  ev.preventDefault();
  const { i, j } = ev.target.dataset;
  const siguiente = matrizEl.querySelector(`[data-i="${Number(i) + 1}"][data-j="${j}"]`);
  (siguiente || ev.target).focus();
  if (!siguiente) ev.target.blur();
});

cargar();