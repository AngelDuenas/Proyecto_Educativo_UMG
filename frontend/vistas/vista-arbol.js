/* =====================================================================
   Vista Estructura académica (P3)
   ---------------------------------------------------------------------
   GET /api/arbol →
     { "raiz": { "nombre": "Universidad Mariano Gálvez", "tipo": "Universidad",
                 "hijos": [ { "nombre": "...", "tipo": "Facultad", "hijos": [ ... ] } ] },
       "recorrido": ["Universidad Mariano Gálvez", "Facultad de Ingeniería", ...] }  // preorden hecho en C++
   Cada nodo: { nombre, tipo, hijos: [] }  (las hojas tienen hijos vacío)
   ===================================================================== */

const lienzo = document.getElementById("lienzo-arbol");
const recorridoEl = document.getElementById("recorrido");
const cifrasEl = document.getElementById("cifras");
const leyendaEl = document.getElementById("leyenda-niveles");
const btnRecorrer = document.getElementById("btn-recorrer");

// Medidas del dibujo
const ANCHO_NODO = 180;
const ALTO_NODO = 42;
const SEPARACION_X = 215; // distancia entre niveles
const SEPARACION_Y = 56; // distancia entre hojas
const MARGEN = 20;

let datos = null;
let nodosDibujados = []; // en preorden: { nodo, x, y, nivel, el }
let animando = false;

async function cargar() {
  try {
    datos = await api("/arbol");
    dibujar();
  } catch (e) {
    lienzo.innerHTML = htmlError(`No se pudo cargar el árbol: ${e.message}`);
  }
}

/* Calcula posiciones: las hojas van una debajo de otra y cada padre
   se coloca a la altura media de sus hijos. */
function calcularPosiciones(raiz) {
  const lista = [];
  let siguienteHoja = 0;
  let profundidad = 0;

  function visitar(nodo, nivel) {
    profundidad = Math.max(profundidad, nivel);
    const item = { nodo, nivel, x: MARGEN + nivel * SEPARACION_X, y: 0, hijos: [] };
    lista.push(item); // preorden
    if (!nodo.hijos || !nodo.hijos.length) {
      item.y = MARGEN + siguienteHoja * SEPARACION_Y;
      siguienteHoja++;
    } else {
      item.hijos = nodo.hijos.map((h) => visitar(h, nivel + 1));
      item.y = (item.hijos[0].y + item.hijos[item.hijos.length - 1].y) / 2;
    }
    return item;
  }

  visitar(raiz, 0);
  return { lista, hojas: siguienteHoja, profundidad };
}

function recortar(texto, max = 24) {
  return texto.length > max ? texto.slice(0, max - 1) + "…" : texto;
}

function dibujar() {
  const { raiz, recorrido } = datos;
  if (!raiz) {
    lienzo.innerHTML = htmlVacio("El árbol está vacío.", " El servidor no devolvió una raíz.");
    return;
  }

  const { lista, hojas, profundidad } = calcularPosiciones(raiz);
  nodosDibujados = lista;
  const ancho = MARGEN * 2 + profundidad * SEPARACION_X + ANCHO_NODO;
  const alto = MARGEN * 2 + Math.max(hojas - 1, 0) * SEPARACION_Y + ALTO_NODO;

  const aristas = lista
    .flatMap((p) =>
      p.hijos.map((h) => {
        const x1 = p.x + ANCHO_NODO;
        const y1 = p.y + ALTO_NODO / 2;
        const x2 = h.x;
        const y2 = h.y + ALTO_NODO / 2;
        const medio = (x1 + x2) / 2;
        return `<path class="arista" d="M${x1},${y1} C${medio},${y1} ${medio},${y2} ${x2},${y2}"/>`;
      })
    )
    .join("");

  const nodos = lista
    .map(
      (p, i) => `
      <g class="nodo nivel-${p.nivel}" data-indice="${i}" transform="translate(${p.x},${p.y})">
        <title>${esc(p.nodo.tipo)}: ${esc(p.nodo.nombre)}</title>
        <rect width="${ANCHO_NODO}" height="${ALTO_NODO}" rx="${p.nivel === 0 ? 4 : 6}"/>
        <text x="12" y="18">${esc(recortar(p.nodo.nombre))}</text>
        <text class="nodo-tipo" x="12" y="33">${esc(p.nodo.tipo || "")}${p.nodo.codigo ? "  " + esc(p.nodo.codigo) : ""}</text>
      </g>`
    )
    .join("");

  lienzo.innerHTML = `
    <svg style="width:100%;max-width:${ancho}px;min-width:${Math.round(ancho * 0.7)}px;height:auto" viewBox="0 0 ${ancho} ${alto}" role="img"
         aria-label="Árbol académico con ${lista.length} nodos">
      ${aristas}${nodos}
    </svg>`;
  lista.forEach((p, i) => (p.el = lienzo.querySelector(`[data-indice="${i}"]`)));

  // Leyenda: un color por nivel, con el tipo de nodo de ese nivel
  const tipos = [];
  lista.forEach((p) => (tipos[p.nivel] = tipos[p.nivel] || p.nodo.tipo));
  const colores = ["var(--color-marino)", "var(--color-azul-700)", "var(--color-celeste)", "var(--color-amarillo)", "#fff"];
  leyendaEl.innerHTML = tipos
    .map((t, n) => `<span><i style="background:${colores[n] || colores[4]};border:1px solid var(--color-marino)"></i>${esc(t || "Nivel " + n)}</span>`)
    .join("");

  cifrasEl.innerHTML = `
    <div class="cifra info"><span class="cifra-texto">Nodos</span><span class="cifra-valor">${lista.length}</span><span class="cifra-detalle">Contando la raíz</span></div>
    <div class="cifra ok"><span class="cifra-texto">Hojas</span><span class="cifra-valor">${hojas}</span><span class="cifra-detalle">Nodos sin hijos (cursos)</span></div>
    <div class="cifra alerta"><span class="cifra-texto">Altura</span><span class="cifra-valor">${profundidad}</span><span class="cifra-detalle">Niveles debajo de la raíz</span></div>`;

  dibujarRecorrido(recorrido || lista.map((p) => p.nodo.nombre));
}

function dibujarRecorrido(nombres) {
  recorridoEl.innerHTML = nombres.map((n, i) => `<li data-paso="${i}"><b>${i + 1}</b>${esc(n)}</li>`).join("");
}

/* Animación: ilumina los nodos en el orden del recorrido que dio el C++ */
btnRecorrer.addEventListener("click", async () => {
  if (animando || !datos) return;
  animando = true;
  btnRecorrer.disabled = true;

  const nombres = datos.recorrido || nodosDibujados.map((p) => p.nodo.nombre);
  const usados = new Set();
  nodosDibujados.forEach((p) => p.el.classList.remove("visitado"));
  recorridoEl.querySelectorAll("li").forEach((li) => li.classList.remove("actual"));

  const pausa = matchMedia("(prefers-reduced-motion: reduce)").matches ? 0 : 380;

  for (let paso = 0; paso < nombres.length; paso++) {
    const indice = nodosDibujados.findIndex((p, i) => !usados.has(i) && p.nodo.nombre === nombres[paso]);
    if (indice >= 0) {
      usados.add(indice);
      nodosDibujados[indice].el.classList.add("visitado");
    }
    recorridoEl.querySelectorAll("li").forEach((li) => li.classList.toggle("actual", Number(li.dataset.paso) === paso));
    await new Promise((r) => setTimeout(r, pausa));
  }

  recorridoEl.querySelectorAll("li").forEach((li) => li.classList.remove("actual"));
  btnRecorrer.disabled = false;
  btnRecorrer.textContent = "Repetir recorrido";
  animando = false;
});

cargar();