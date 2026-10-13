/* =====================================================================
   Vista Reportes (P4)
   ---------------------------------------------------------------------
   GET /api/reportes →
     { "rankingNotas":      [ { "carnet": "...", "nombre": "...", "promedio": 88.4 } ],   // ya ordenado de mayor a menor
       "rankingAsistencia": [ { "carnet": "...", "nombre": "...", "porcentaje": 100 } ] } // ya ordenado de mayor a menor
   GET /api/cursos                      → para llenar el selector
   GET /api/grafo/bfs?origen=MAT101     → { "origen": "MAT101", "orden": ["MAT101","MAT102",...] }  (módulo de P3)
   ===================================================================== */

const rankNotasEl = document.getElementById("ranking-notas");
const rankAsisEl = document.getElementById("ranking-asistencia");
const ordenNotas = document.getElementById("orden-notas");
const ordenAsis = document.getElementById("orden-asis");
const selCurso = document.getElementById("curso-origen");
const relacionadosEl = document.getElementById("relacionados");

let reporte = null;
let cursos = [];

async function cargar() {
  try {
    [reporte, cursos] = await Promise.all([api("/reportes"), api("/cursos")]);
    dibujarRankings();
    llenarCursos();
  } catch (e) {
    const msg = htmlError(`No se pudo cargar el reporte: ${e.message}`);
    rankNotasEl.innerHTML = msg;
    rankAsisEl.innerHTML = msg;
    relacionadosEl.innerHTML = "";
  }
}

function tablaRanking(lista, columna, celdaValor) {
  if (!lista.length) return htmlVacio("Sin datos.", " Todavía no hay información para este ranking.");
  return `
    <div class="tabla-contenedor"><table class="tabla">
      <thead><tr><th class="numero">Puesto</th><th>Estudiante</th><th>${columna}</th></tr></thead>
      <tbody>${lista
        .map(
          (r) => `<tr>
            <td class="numero">${r.puesto}</td>
            <td>${esc(r.nombre)}<span class="sub">${esc(r.carnet)}</span></td>
            <td>${celdaValor(r)}</td>
          </tr>`
        )
        .join("")}</tbody>
    </table></div>`;
}

// El puesto siempre es el del ranking original, aunque se invierta la vista
function conPuesto(lista, orden) {
  const numerada = lista.map((r, i) => ({ ...r, puesto: i + 1 }));
  return orden === "asc" ? numerada.reverse() : numerada;
}

function dibujarRankings() {
  rankNotasEl.innerHTML = tablaRanking(
    conPuesto(reporte.rankingNotas, ordenNotas.value),
    "Promedio",
    (r) => {
      const estado = r.promedio >= CONFIG.NOTA_APROBACION ? '<span class="etiqueta ok">Aprobando</span>' : '<span class="etiqueta mal">En riesgo</span>';
      return `<strong>${formatear(r.promedio)}</strong> ${estado}`;
    }
  );
  rankAsisEl.innerHTML = tablaRanking(conPuesto(reporte.rankingAsistencia, ordenAsis.value), "Asistencia", (r) => barra(r.porcentaje));
}

function llenarCursos() {
  if (!cursos.length) {
    selCurso.innerHTML = '<option value="">Sin cursos</option>';
    relacionadosEl.innerHTML = htmlVacio("No hay cursos.", " Regístralos en Docentes y cursos.");
    return;
  }
  selCurso.innerHTML = cursos.map((c) => `<option value="${esc(c.codigo)}">${esc(c.codigo)}  ${esc(c.nombre)}</option>`).join("");
  cargarRelacionados();
}

async function cargarRelacionados() {
  const origen = selCurso.value;
  if (!origen) return;
  relacionadosEl.innerHTML = '<div class="cargando">Recorriendo el grafo…</div>';
  try {
    const { orden } = await api(`/grafo/bfs?origen=${encodeURIComponent(origen)}`);
    const nombre = (c) => (cursos.find((x) => x.codigo === c) || {}).nombre || c;
    const dependientes = orden.filter((c) => c !== origen);

    if (!dependientes.length) {
      relacionadosEl.innerHTML = htmlVacio(`${nombre(origen)} no es prerrequisito de ningún curso.`, " Prueba con un curso de los primeros semestres.");
      return;
    }
    relacionadosEl.innerHTML = `
      <p class="dimension mb-3">Aprobar ${esc(nombre(origen))} abre el camino a ${dependientes.length} ${dependientes.length === 1 ? "curso" : "cursos"}:</p>
      <ol class="recorrido-lista">${dependientes
        .map((c, i) => `<li><b>${i + 1}</b>${esc(c)}  ${esc(nombre(c))}</li>`)
        .join("")}</ol>`;
  } catch (e) {
    relacionadosEl.innerHTML = htmlError(`No se pudo recorrer el grafo: ${e.message}`);
  }
}

ordenNotas.addEventListener("change", dibujarRankings);
ordenAsis.addEventListener("change", dibujarRankings);
selCurso.addEventListener("change", cargarRelacionados);
document.getElementById("btn-imprimir").addEventListener("click", () => window.print());

cargar();