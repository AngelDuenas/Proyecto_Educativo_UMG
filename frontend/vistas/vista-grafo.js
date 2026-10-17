/* =====================================================================
   Vista Prerrequisitos (P3)
   ---------------------------------------------------------------------
   GET /api/grafo →
     { "nodos":  [ { "codigo": "MAT101", "nombre": "Matemática I" }, ... ],
       "matriz": [ [0,1,0,...], ... ] }   // matriz[i][j] = 1 si nodos[i] es prerrequisito de nodos[j]
   GET /api/grafo/bfs?origen=MAT101 → { "origen": "MAT101", "orden": ["MAT101","MAT102","FIS101",...] }
   GET /api/grafo/dfs?origen=MAT101 → { "origen": "MAT101", "orden": [...] }
   ===================================================================== */

const lienzo = document.getElementById("lienzo-grafo");
const resultadoEl = document.getElementById("resultado");
const selOrigen = document.getElementById("origen");
const matrizEl = document.getElementById("matriz-adyacencia");
const dimensionEl = document.getElementById("dimension");
const botones = document.querySelectorAll("[data-recorrido]");

const RADIO = 32;
const SEP_X = 190;
const SEP_Y = 105;
const MARGEN = 90;

let grafo = null;
let posiciones = []; // { x, y } por índice de nodo
let animando = false;

async function cargar() {
  try {
    grafo = await api("/grafo");
    if (!grafo.nodos.length) {
      lienzo.innerHTML = htmlVacio("No hay cursos en el grafo.", " Regístralos en Docentes y cursos.");
      return;
    }
    selOrigen.innerHTML = grafo.nodos
      .map((n) => `<option value="${esc(n.codigo)}">${esc(n.codigo)}  ${esc(n.nombre)}</option>`)
      .join("");
    dibujarGrafo();
    dibujarMatriz();
  } catch (e) {
    lienzo.innerHTML = htmlError(`No se pudo cargar el grafo: ${e.message}`);
  }
}

/* Nivel de cada curso = camino más largo desde un curso sin prerrequisitos.
   Así los cursos base quedan a la izquierda y los avanzados a la derecha. */
function calcularNiveles() {
  const n = grafo.nodos.length;
  const nivel = new Array(n).fill(0);
  for (let vuelta = 0; vuelta < n; vuelta++) {
    let cambio = false;
    for (let i = 0; i < n; i++)
      for (let j = 0; j < n; j++)
        if (grafo.matriz[i][j] && nivel[j] < nivel[i] + 1) {
          nivel[j] = nivel[i] + 1;
          cambio = true;
        }
    if (!cambio) break; // si hay ciclos, se corta solo
  }
  return nivel;
}

function dibujarGrafo() {
  const { nodos, matriz } = grafo;
  const nivel = calcularNiveles();
  const porNivel = [];
  nodos.forEach((_, i) => (porNivel[nivel[i]] = porNivel[nivel[i]] || []).push(i));
  const maxFilas = Math.max(...porNivel.map((l) => (l ? l.length : 0)));

  posiciones = [];
  porNivel.forEach((indices, col) => {
    if (!indices) return;
    const desplazamiento = ((maxFilas - indices.length) * SEP_Y) / 2; // centrar columna
    indices.forEach((i, fila) => {
      posiciones[i] = { x: MARGEN + col * SEP_X, y: 60 + desplazamiento + fila * SEP_Y };
    });
  });

  const ancho = MARGEN * 2 + (porNivel.length - 1) * SEP_X;
  const alto = 60 * 2 + (maxFilas - 1) * SEP_Y + 20;

  let aristas = "";
  matriz.forEach((fila, i) =>
    fila.forEach((v, j) => {
      if (!v) return;
      const a = posiciones[i];
      const b = posiciones[j];
      const dx = b.x - a.x;
      const dy = b.y - a.y;
      const d = Math.hypot(dx, dy) || 1;
      const x1 = a.x + (dx / d) * RADIO;
      const y1 = a.y + (dy / d) * RADIO;
      const x2 = b.x - (dx / d) * (RADIO + 6);
      const y2 = b.y - (dy / d) * (RADIO + 6);
      aristas += `<line class="arista" data-arista="${i}-${j}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" marker-end="url(#flecha)"/>`;
    })
  );

  const circulos = nodos
    .map((nodo, i) => {
      const { x, y } = posiciones[i];
      const corto = nodo.nombre.length > 20 ? nodo.nombre.slice(0, 19) + "…" : nodo.nombre;
      return `
        <g class="nodo" data-nodo="${i}">
          <title>${esc(nodo.codigo)}: ${esc(nodo.nombre)}</title>
          <circle cx="${x}" cy="${y}" r="${RADIO}"/>
          <text x="${x}" y="${y + 4}" text-anchor="middle" font-weight="700" font-size="12">${esc(nodo.codigo)}</text>
          <text class="nodo-nombre" x="${x}" y="${y + RADIO + 16}" text-anchor="middle">${esc(corto)}</text>
        </g>`;
    })
    .join("");

  lienzo.innerHTML = `
    <svg style="width:100%;max-width:${ancho}px;min-width:${Math.round(ancho * 0.7)}px;height:auto" viewBox="0 0 ${ancho} ${alto}" role="img"
         aria-label="Grafo de prerrequisitos con ${nodos.length} cursos">
      <defs>
        <marker id="flecha" viewBox="0 0 10 10" refX="5" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
          <path d="M0,0 L10,5 L0,10 z" fill="#7fa6b7"/>
        </marker>
        <marker id="flecha-activa" viewBox="0 0 10 10" refX="5" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
          <path d="M0,0 L10,5 L0,10 z" fill="#c2500a"/>
        </marker>
      </defs>
      <g id="capa-aristas">${aristas}</g>
      <g id="capa-nodos">${circulos}</g>
      <g id="capa-orden"></g>
    </svg>`;
}

function dibujarMatriz() {
  const { nodos, matriz } = grafo;
  dimensionEl.textContent = `${nodos.length} × ${nodos.length}`;
  const cabecera = nodos.map((n) => `<th class="celda" title="${esc(n.nombre)}">${esc(n.codigo)}</th>`).join("");
  const filas = matriz
    .map(
      (fila, i) => `<tr>
        <td><strong>${esc(nodos[i].codigo)}</strong></td>
        ${fila.map((v, j) => `<td class="celda ${v ? "uno" : "cero"}" title="matriz[${i}][${j}]">${v}</td>`).join("")}
      </tr>`
    )
    .join("");
  matrizEl.innerHTML = `
    <div class="tabla-contenedor">
      <table class="tabla matriz adyacencia">
        <thead><tr><th>De \\ hacia</th>${cabecera}</tr></thead>
        <tbody>${filas}</tbody>
      </table>
    </div>`;
}

function limpiarRecorrido() {
  lienzo.querySelectorAll(".nodo").forEach((g) => g.classList.remove("visitado", "origen"));
  lienzo.querySelectorAll(".arista").forEach((a) => {
    a.classList.remove("recorrida");
    a.setAttribute("marker-end", "url(#flecha)");
  });
  const capa = lienzo.querySelector("#capa-orden");
  if (capa) capa.innerHTML = "";
}

/* Arista por la que se llegó a cada nodo:
   BFS → el primer nodo visitado que apunta a él
   DFS → el último nodo visitado (antes que él) que apunta a él */
function aristaDeLlegada(indices, paso, tipo) {
  const destino = indices[paso];
  const anteriores = indices.slice(0, paso).filter((i) => grafo.matriz[i][destino]);
  if (!anteriores.length) return null;
  return tipo === "bfs" ? anteriores[0] : anteriores[anteriores.length - 1];
}

async function recorrer(tipo) {
  if (animando || !grafo) return;
  const origen = selOrigen.value;
  animando = true;
  botones.forEach((b) => (b.disabled = true));
  limpiarRecorrido();

  try {
    const { orden } = await api(`/grafo/${tipo}?origen=${encodeURIComponent(origen)}`);
    const indice = Object.fromEntries(grafo.nodos.map((n, i) => [n.codigo, i]));
    const indices = orden.map((c) => indice[c]).filter((i) => i !== undefined);
    const nombre = tipo === "bfs" ? "BFS (anchura)" : "DFS (profundidad)";

    resultadoEl.innerHTML = `
      <p class="dimension mb-2.5">Orden de visita con ${nombre} desde ${esc(origen)}:</p>
      <ol class="recorrido-lista">${orden.map((c, i) => `<li data-paso="${i}"><b>${i + 1}</b>${esc(c)}</li>`).join("")}</ol>`;

    const capa = lienzo.querySelector("#capa-orden");
    const pausa = matchMedia("(prefers-reduced-motion: reduce)").matches ? 0 : 550;

    for (let paso = 0; paso < indices.length; paso++) {
      const i = indices[paso];
      const g = lienzo.querySelector(`[data-nodo="${i}"]`);
      g.classList.add(paso === 0 ? "origen" : "visitado");

      const desde = aristaDeLlegada(indices, paso, tipo);
      if (desde !== null) {
        const linea = lienzo.querySelector(`[data-arista="${desde}-${i}"]`);
        if (linea) {
          linea.classList.add("recorrida");
          linea.setAttribute("marker-end", "url(#flecha-activa)");
        }
      }

      const { x, y } = posiciones[i];
      capa.insertAdjacentHTML(
        "beforeend",
        `<g class="nodo-orden"><circle cx="${x + RADIO - 4}" cy="${y - RADIO + 4}" r="10"/>
         <text x="${x + RADIO - 4}" y="${y - RADIO + 8}" text-anchor="middle">${paso + 1}</text></g>`
      );
      resultadoEl.querySelectorAll("li").forEach((li) => li.classList.toggle("actual", Number(li.dataset.paso) === paso));
      await new Promise((r) => setTimeout(r, pausa));
    }
    resultadoEl.querySelectorAll("li").forEach((li) => li.classList.remove("actual"));

    if (orden.length === 1) {
      resultadoEl.insertAdjacentHTML("beforeend", `<p class="dimension mt-2.5">${esc(origen)} no es prerrequisito de ningún curso.</p>`);
    }
  } catch (e) {
    mostrarError(e, "No se pudo hacer el recorrido");
  } finally {
    animando = false;
    botones.forEach((b) => (b.disabled = false));
  }
}

botones.forEach((b) => b.addEventListener("click", () => recorrer(b.dataset.recorrido)));
selOrigen.addEventListener("change", () => {
  limpiarRecorrido();
  resultadoEl.innerHTML = '<p class="dimension">Elige un tipo de recorrido.</p>';
});

cargar();