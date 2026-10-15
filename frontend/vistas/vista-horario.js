/* =====================================================================
   Vista Horarios
   ---------------------------------------------------------------------
   GET /api/horario[?carnet=X] →
     { dias: ["Lunes", ...], periodos: [ {inicio: "07:00", fin: "07:50"}, ... ],
       celdas: [dia][periodo] → [ {codigo, nombre, semestre, docentes: [nombres]} ],
       choques: [ {dia, periodo, tipo: "docente"|"estudiante", quien, cursos: [codigos]} ],
       bloquesPorCurso: [ {codigo, nombre, creditos, bloques} ] }
   PUT /api/horario   body: { dia, periodo, cursos: [codigos] }   → { ..., choques }
   ===================================================================== */

const MAX_POR_BLOQUE = 3;
const selFiltro = document.getElementById("filtro");
const form = document.getElementById("form-bloque");
const modal = document.getElementById("modal-bloque");

let horario = null;
let cursos = [];
let editando = null; // { dia, periodo }

// En celular y tablet se ve un día a la vez (por defecto hoy, si es de lunes a viernes).
const pantallaChica = matchMedia("(max-width: 1023px)");
let diaMovil = Math.min(Math.max(new Date().getDay() - 1, 0), 4);

async function cargarFiltros() {
  try {
    const [estudiantes, lista] = await Promise.all([api("/estudiantes?orden=nombre"), api("/cursos")]);
    cursos = lista;
    selFiltro.innerHTML =
      '<option value="">Todos los cursos</option>' +
      estudiantes.map((e) => `<option value="${esc(e.carnet)}">${esc(e.nombre)} (solo sus cursos)</option>`).join("");
  } catch (e) {
    mostrarError(e, "No se pudieron cargar los estudiantes");
  }
  await cargar();
}

async function cargar() {
  const carnet = selFiltro.value;
  try {
    horario = await api(carnet ? `/horario?carnet=${encodeURIComponent(carnet)}` : "/horario");
    dibujar();
  } catch (e) {
    document.getElementById("tabla-horario").innerHTML = htmlError(`No se pudo cargar el horario: ${e.message}`);
  }
}

const tieneChoque = (d, p) => horario.choques.some((c) => c.dia === d && c.periodo === p);

function dibujar() {
  const filtrando = Boolean(selFiltro.value);
  document.getElementById("ayuda-horario").textContent = filtrando
    ? "Horario personal: solo los cursos en los que está inscrito. Para editar, vuelve a «Todos los cursos»."
    : "Clic en un bloque para editarlo.";

  // Leyenda por semestre
  const semestres = [...new Set(cursos.map((c) => c.semestre))].sort((a, b) => a - b);
  document.getElementById("leyenda-semestres").innerHTML = semestres
    .map((s) => `<span><i class="sem-${((s - 1) % 4) + 1}"></i>Semestre ${esc(s)}</span>`)
    .join("");

  if (pantallaChica.matches) {
    dibujarDia(filtrando);
  } else {
    dibujarSemana(filtrando);
  }
  dibujarCifras();
  dibujarChoques();
  dibujarCarga();
}

// Cursos de un bloque como "chips" de color por semestre
function chipsBloque(bloque) {
  return bloque.length
    ? bloque
        .map(
          (c) => `<span class="bloque-curso sem-${((c.semestre - 1) % 4) + 1}" title="${esc(c.nombre)}${c.docentes.length ? " · " + esc(c.docentes.join(", ")) : ""}">
            <b>${esc(c.codigo)}</b><small>${esc(c.nombre)}</small></span>`
        )
        .join("")
    : '<span class="bloque-vacio">Libre</span>';
}

function atributosEditar(filtrando, d, p) {
  if (filtrando) return "";
  return ` data-dia="${d}" data-periodo="${p}" tabindex="0" role="button" aria-label="Editar ${esc(horario.dias[d])} ${esc(horario.periodos[p].inicio)}"`;
}

/* Celular: botones para elegir el día y la lista de sus periodos */
function dibujarDia(filtrando) {
  const pestanas = horario.dias
    .map(
      (d, i) => `<button class="pestana" type="button" role="tab" data-dia-movil="${i}" aria-selected="${i === diaMovil}">${esc(d.slice(0, 3))}</button>`
    )
    .join("");
  const filas = horario.periodos
    .map((per, p) => {
      const choque = tieneChoque(diaMovil, p);
      const receso = p === 3 ? '<li class="fila-receso-movil">Receso · 09:30 a 09:50</li>' : "";
      return `${receso}<li class="periodo-movil celda-horario${choque ? " choque" : ""}${filtrando ? "" : " editable"}"${atributosEditar(filtrando, diaMovil, p)}>
        <span class="hora-movil">${esc(per.inicio)}<small>${esc(per.fin)}</small></span>
        <span class="min-w-0 flex-1">${chipsBloque(horario.celdas[diaMovil][p])}${choque ? '<span class="etiqueta mal">Choque</span>' : ""}</span>
      </li>`;
    })
    .join("");
  document.getElementById("tabla-horario").innerHTML = `
    <div class="pestanas w-full justify-between" role="tablist" aria-label="Día">${pestanas}</div>
    <ol class="grid gap-2">${filas}</ol>`;
}

/* Escritorio y tablet: la matriz completa días × periodos */
function dibujarSemana(filtrando) {
  const cabecera = horario.dias.map((d) => `<th class="text-center">${esc(d)}</th>`).join("");
  const filas = horario.periodos
    .map((per, p) => {
      const celdas = horario.dias
        .map((_, d) => {
          const choque = tieneChoque(d, p);
          return `<td class="celda-horario${choque ? " choque" : ""}${filtrando ? "" : " editable"}"${atributosEditar(filtrando, d, p)}>${chipsBloque(horario.celdas[d][p])}${choque ? '<span class="etiqueta mal mt-1">Choque</span>' : ""}</td>`;
        })
        .join("");
      // Receso entre el tercer y el cuarto periodo
      const receso = p === 3 ? `<tr class="fila-receso"><td colspan="${horario.dias.length + 1}">Receso · 09:30 a 09:50</td></tr>` : "";
      return `${receso}<tr><th class="hora">${esc(per.inicio)}<span class="sub">${esc(per.fin)}</span></th>${celdas}</tr>`;
    })
    .join("");

  document.getElementById("tabla-horario").innerHTML = `
    <div class="tabla-contenedor"><table class="tabla horario">
      <thead><tr><th>Hora</th>${cabecera}</tr></thead>
      <tbody>${filas}</tbody>
    </table></div>`;
}

function dibujarCifras() {
  const ocupados = horario.celdas.flat().filter((b) => b.length).length;
  const total = horario.dias.length * horario.periodos.length;
  const sinBloques = horario.bloquesPorCurso.filter((c) => c.bloques === 0).length;
  document.getElementById("cifras").innerHTML = `
    <div class="cifra info"><span class="cifra-texto">Bloques ocupados</span><span class="cifra-valor">${ocupados} / ${total}</span><span class="cifra-detalle">De la matriz ${horario.dias.length} × ${horario.periodos.length}</span></div>
    <div class="cifra ${horario.choques.length ? "mal" : "ok"}"><span class="cifra-texto">Choques</span><span class="cifra-valor">${horario.choques.length}</span><span class="cifra-detalle">${horario.choques.length ? "Revisa la lista de abajo" : "Ningún choque"}</span></div>
    <div class="cifra ${sinBloques ? "alerta" : "ok"}"><span class="cifra-texto">Cursos sin horario</span><span class="cifra-valor">${sinBloques}</span><span class="cifra-detalle">Sin ningún bloque asignado</span></div>`;
}

function dibujarChoques() {
  const el = document.getElementById("lista-choques");
  if (!horario.choques.length) {
    el.innerHTML = htmlVacio("Sin choques.", " Nadie tiene dos cursos a la misma hora.");
    return;
  }
  el.innerHTML = `<ul class="grid gap-2">${horario.choques
    .map(
      (c) => `<li class="aviso-error">
        <strong>${esc(horario.dias[c.dia])} ${esc(horario.periodos[c.periodo].inicio)}</strong> ·
        ${c.tipo === "docente" ? "El docente" : "El estudiante"} <b>${esc(c.quien)}</b> tiene ${c.cursos.map(esc).join(" y ")} al mismo tiempo.
      </li>`
    )
    .join("")}</ul>`;
}

function dibujarCarga() {
  const el = document.getElementById("tabla-carga");
  if (!horario.bloquesPorCurso.length) {
    el.innerHTML = htmlVacio("No hay cursos que mostrar.", "");
    return;
  }
  el.innerHTML = `
    <div class="tabla-contenedor"><table class="tabla">
      <thead><tr><th>Curso</th><th class="numero">Créditos</th><th class="numero">Bloques</th><th></th></tr></thead>
      <tbody>${horario.bloquesPorCurso
        .map((c) => {
          const etiqueta = c.bloques === 0
            ? '<span class="etiqueta mal">Sin horario</span>'
            : c.bloques < c.creditos - 2
              ? '<span class="etiqueta alerta">Pocos bloques</span>'
              : '<span class="etiqueta ok">Bien</span>';
          return `<tr><td>${esc(c.codigo)}<span class="sub">${esc(c.nombre)}</span></td>
            <td class="numero">${esc(c.creditos)}</td><td class="numero">${esc(c.bloques)}</td><td>${etiqueta}</td></tr>`;
        })
        .join("")}</tbody>
    </table></div>`;
}

/* ---------- Editar un bloque ---------- */
function abrirBloque(dia, periodo) {
  editando = { dia, periodo };
  const actuales = horario.celdas[dia][periodo].map((c) => c.codigo);
  document.getElementById("titulo-modal-bloque").textContent =
    `${horario.dias[dia]} · ${horario.periodos[periodo].inicio} a ${horario.periodos[periodo].fin}`;
  document.getElementById("opciones-bloque").innerHTML = [...cursos]
    .sort((a, b) => a.semestre - b.semestre || a.codigo.localeCompare(b.codigo))
    .map(
      (c) => `<label class="opcion-curso">
        <input type="checkbox" name="cursos" value="${esc(c.codigo)}"${actuales.includes(c.codigo) ? " checked" : ""}>
        <span><b>${esc(c.codigo)}</b> ${esc(c.nombre)}</span>
      </label>`
    )
    .join("");
  modal.showModal();
}

document.getElementById("tabla-horario").addEventListener("click", (ev) => {
  const dia = ev.target.closest("[data-dia-movil]");
  if (dia) {
    diaMovil = Number(dia.dataset.diaMovil);
    dibujarDia(Boolean(selFiltro.value));
    return;
  }
  const celda = ev.target.closest("[data-dia]");
  if (celda) abrirBloque(Number(celda.dataset.dia), Number(celda.dataset.periodo));
});
document.getElementById("tabla-horario").addEventListener("keydown", (ev) => {
  const celda = ev.target.closest("[data-dia]");
  if (celda && (ev.key === "Enter" || ev.key === " ")) {
    ev.preventDefault();
    abrirBloque(Number(celda.dataset.dia), Number(celda.dataset.periodo));
  }
});

// No dejar marcar más de 3 cursos
form.addEventListener("change", () => {
  const marcados = form.querySelectorAll('input[name="cursos"]:checked').length;
  form.querySelectorAll('input[name="cursos"]').forEach((c) => (c.disabled = !c.checked && marcados >= MAX_POR_BLOQUE));
});

form.addEventListener("submit", async (ev) => {
  ev.preventDefault();
  const elegidos = [...form.querySelectorAll('input[name="cursos"]:checked')].map((c) => c.value);
  const boton = form.querySelector('[type="submit"]');
  boton.disabled = true;
  try {
    const r = await api("/horario", "PUT", { ...editando, cursos: elegidos });
    modal.close();
    if (r.choques && r.choques.length) toast(`Guardado, pero hay ${r.choques.length === 1 ? "un choque" : r.choques.length + " choques"} en este bloque.`, "info");
    else toast("Bloque guardado.");
    await cargar();
  } catch (e) {
    mostrarError(e, "No se pudo guardar el bloque");
  } finally {
    boton.disabled = false;
  }
});

selFiltro.addEventListener("change", cargar);
pantallaChica.addEventListener("change", () => horario && dibujar());

cargarFiltros();
