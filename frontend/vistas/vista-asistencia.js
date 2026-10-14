/* =====================================================================
   Vista Asistencia (P4)
   ---------------------------------------------------------------------
   GET /api/asistencia →
     { "dias": ["L","M","X","J","V"],
       "estudiantes": [ { "carnet": "...", "nombre": "..." } ],
       "matriz": [ [1,1,0,1,1], ... ],        // asistencia[estudiante][día], 1 = presente
       "porEstudiante": [80.0, 100.0, ...],   // % por fila
       "porDia": [87.5, 100.0, ...] }         // % por columna
   PUT /api/asistencia  body: { "carnet": "...", "dia": 2, "valor": 0 }
   ===================================================================== */

const NOMBRES_DIA = { L: "Lunes", M: "Martes", X: "Miércoles", J: "Jueves", V: "Viernes" };

const matrizEl = document.getElementById("matriz-asistencia");
const cifrasEl = document.getElementById("cifras");
const dimensionEl = document.getElementById("dimension");

let datos = null;

async function cargar() {
  try {
    datos = await api("/asistencia");
    dibujarMatriz();
    dibujarCifras();
  } catch (e) {
    matrizEl.innerHTML = htmlError(`No se pudo cargar la asistencia: ${e.message}`);
  }
}

function dibujarCifras() {
  const { dias, estudiantes, porEstudiante, porDia } = datos;
  if (!estudiantes.length) {
    cifrasEl.innerHTML = "";
    return;
  }
  const general = porDia.reduce((a, b) => a + b, 0) / porDia.length;
  const peorDia = porDia.indexOf(Math.min(...porDia));
  const enRiesgo = estudiantes.filter((_, i) => porEstudiante[i] < CONFIG.ASISTENCIA_MINIMA);

  cifrasEl.innerHTML = `
    <div class="cifra info">
      <span class="cifra-texto">Asistencia general</span>
      <span class="cifra-valor">${formatear(general, 1)} %</span>
      <span class="cifra-detalle">Promedio de los ${dias.length} días</span>
    </div>
    <div class="cifra alerta">
      <span class="cifra-texto">Día con más ausencias</span>
      <span class="cifra-valor">${NOMBRES_DIA[dias[peorDia]] || dias[peorDia]}</span>
      <span class="cifra-detalle">${formatear(porDia[peorDia], 0)} % de asistencia</span>
    </div>
    <div class="cifra ${enRiesgo.length ? "mal" : "ok"}">
      <span class="cifra-texto">Bajo ${CONFIG.ASISTENCIA_MINIMA} % de asistencia</span>
      <span class="cifra-valor">${enRiesgo.length}</span>
      <span class="cifra-detalle">${
        enRiesgo.length ? enRiesgo.slice(0, 3).map((e) => esc(e.nombre)).join(", ") + (enRiesgo.length > 3 ? "…" : "") : "Nadie en riesgo esta semana"
      }</span>
    </div>`;
}

function dibujarMatriz() {
  const { dias, estudiantes, matriz } = datos;
  dimensionEl.textContent = `${estudiantes.length} × ${dias.length} (estudiantes × días)`;

  if (!estudiantes.length) {
    matrizEl.innerHTML = htmlVacio("No hay estudiantes.", " Regístralos en el módulo Estudiantes.");
    return;
  }

  const encabezado = dias
    .map((d) => `<th class="celda" title="${NOMBRES_DIA[d] || d}"><abbr title="${NOMBRES_DIA[d] || d}">${d}</abbr></th>`)
    .join("");

  const filas = estudiantes
    .map((e, i) => {
      const celdas = dias
        .map((d, j) => {
          const presente = matriz[i][j] === 1;
          return `<td class="celda">
            <button class="marca-asistencia" type="button" data-i="${i}" data-j="${j}"
              aria-pressed="${presente}" title="asistencia[${i}][${j}]"
              aria-label="${esc(e.nombre)}, ${NOMBRES_DIA[d] || d}: ${presente ? "presente" : "ausente"}">
              ${presente ? "✓" : "✗"}
            </button>
          </td>`;
        })
        .join("");
      return `<tr>
        <td>${esc(e.nombre)}<span class="sub">${esc(e.carnet)}</span></td>
        ${celdas}
        <td data-pct-fila="${i}"></td>
      </tr>`;
    })
    .join("");

  const pie = dias.map((_, j) => `<td class="celda" data-pct-dia="${j}"></td>`).join("");

  matrizEl.innerHTML = `
    <div class="tabla-contenedor">
      <table class="tabla matriz">
        <thead><tr><th>Estudiante</th>${encabezado}<th>Asistencia</th></tr></thead>
        <tbody>${filas}</tbody>
        <tfoot><tr><td>Asistencia del día</td>${pie}<td></td></tr></tfoot>
      </table>
    </div>`;

  dibujarPorcentajes();
}

function dibujarPorcentajes() {
  datos.porEstudiante.forEach((p, i) => {
    const celda = matrizEl.querySelector(`[data-pct-fila="${i}"]`);
    if (celda) celda.innerHTML = barra(p);
  });
  datos.porDia.forEach((p, j) => {
    const celda = matrizEl.querySelector(`[data-pct-dia="${j}"]`);
    if (celda) celda.textContent = `${formatear(p, 0)}%`;
  });
}

matrizEl.addEventListener("click", async (ev) => {
  const boton = ev.target.closest(".marca-asistencia");
  if (!boton) return;
  const i = Number(boton.dataset.i);
  const j = Number(boton.dataset.j);
  const nuevo = datos.matriz[i][j] === 1 ? 0 : 1;
  const estudiante = datos.estudiantes[i];
  const dia = NOMBRES_DIA[datos.dias[j]] || datos.dias[j];

  boton.disabled = true;
  try {
    await api("/asistencia", "PUT", { carnet: estudiante.carnet, dia: j, valor: nuevo });
    // Pedimos de nuevo los porcentajes que calcula el C++
    datos = await api("/asistencia");
    boton.setAttribute("aria-pressed", String(nuevo === 1));
    boton.setAttribute("aria-label", `${estudiante.nombre}, ${dia}: ${nuevo ? "presente" : "ausente"}`);
    boton.textContent = nuevo ? "✓" : "✗";
    dibujarPorcentajes();
    dibujarCifras();
  } catch (e) {
    mostrarError(e, "No se guardó la asistencia");
  } finally {
    boton.disabled = false;
    boton.focus();
  }
});

cargar();