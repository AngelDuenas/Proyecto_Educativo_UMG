/* =====================================================================
   Vista Avisos y calendario
   ---------------------------------------------------------------------
   GET    /api/avisos       → [ { id, titulo, descripcion, fecha: "AAAA-MM-DD",
                                  tipo: "examen"|"entrega"|"evento"|"aviso", curso, cursoNombre } ]  (por fecha)
   POST   /api/avisos       body: { titulo, descripcion, fecha, tipo, curso }
   PUT    /api/avisos/{id}  body: igual
   DELETE /api/avisos/{id}
   ===================================================================== */

const TIPOS = {
  examen: { texto: "Examen", clase: "mal" },
  entrega: { texto: "Entrega", clase: "alerta" },
  evento: { texto: "Evento", clase: "ok" },
  aviso: { texto: "Aviso", clase: "info" },
};
const MESES = ["ene", "feb", "mar", "abr", "may", "jun", "jul", "ago", "sep", "oct", "nov", "dic"];

const modal = document.getElementById("modal-aviso");
const form = document.getElementById("form-aviso");
const selFiltro = document.getElementById("filtro-tipo");

let avisos = [];
let editando = null; // id del aviso que se edita, o null si es nuevo

// Fecha de hoy como "AAAA-MM-DD" (hora local)
function hoyISO() {
  const h = new Date();
  return `${h.getFullYear()}-${String(h.getMonth() + 1).padStart(2, "0")}-${String(h.getDate()).padStart(2, "0")}`;
}

function diasHasta(fecha) {
  const [a, m, d] = fecha.split("-").map(Number);
  const [ha, hm, hd] = hoyISO().split("-").map(Number);
  return Math.round((Date.UTC(a, m - 1, d) - Date.UTC(ha, hm - 1, hd)) / 86400000);
}

function cuando(fecha) {
  const n = diasHasta(fecha);
  if (n === 0) return "Hoy";
  if (n === 1) return "Mañana";
  if (n > 1) return `En ${n} días`;
  if (n === -1) return "Ayer";
  return `Hace ${-n} días`;
}

async function cargar() {
  try {
    const [lista, cursos] = await Promise.all([api("/avisos"), api("/cursos")]);
    avisos = lista;
    document.getElementById("av-curso").innerHTML =
      '<option value="">General</option>' +
      cursos.map((c) => `<option value="${esc(c.codigo)}">${esc(c.codigo)} · ${esc(c.nombre)}</option>`).join("");
    dibujar();
  } catch (e) {
    document.getElementById("lista-proximos").innerHTML = htmlError(`No se pudieron cargar los avisos: ${e.message}`);
  }
}

function tarjeta(a) {
  const [, m, d] = a.fecha.split("-");
  const tipo = TIPOS[a.tipo] || { texto: a.tipo, clase: "" };
  const n = diasHasta(a.fecha);
  const pronto = n >= 0 && n <= 7;
  return `<li class="aviso-item${pronto ? " pronto" : ""}">
    <div class="aviso-fecha"><b>${Number(d)}</b><span>${MESES[Number(m) - 1]}</span></div>
    <div class="min-w-0 flex-1">
      <div class="flex flex-wrap items-center gap-2">
        <span class="etiqueta ${tipo.clase}">${tipo.texto}</span>
        ${a.curso ? `<span class="etiqueta">${esc(a.curso)}</span>` : ""}
        <span class="text-sm ${pronto ? "font-bold text-marino" : "text-tinta-suave"}">${cuando(a.fecha)}</span>
      </div>
      <strong class="mt-1 block font-titulo text-lg text-marino">${esc(a.titulo)}</strong>
      ${a.descripcion ? `<p class="text-sm text-tinta-suave">${esc(a.descripcion)}</p>` : ""}
      ${a.cursoNombre ? `<p class="text-sm text-tinta-suave">Curso: ${esc(a.cursoNombre)}</p>` : ""}
    </div>
    <div class="flex w-full flex-none justify-end gap-1.5 sm:w-auto sm:self-start">
      <button class="btn btn-secundario btn-chico" type="button" data-editar="${a.id}">Editar</button>
      <button class="btn btn-peligro btn-chico" type="button" data-borrar="${a.id}">Eliminar</button>
    </div>
  </li>`;
}

function dibujar() {
  const tipo = selFiltro.value;
  const filtrados = avisos.filter((a) => !tipo || a.tipo === tipo);
  const proximos = filtrados.filter((a) => diasHasta(a.fecha) >= 0);
  const pasados = filtrados.filter((a) => diasHasta(a.fecha) < 0).reverse();

  const futuros = avisos.filter((a) => diasHasta(a.fecha) >= 0);
  const semana = futuros.filter((a) => diasHasta(a.fecha) <= 7).length;
  const siguiente = futuros[0];
  document.getElementById("cifras").innerHTML = `
    <div class="cifra ${semana ? "alerta" : "ok"}"><span class="cifra-texto">Esta semana</span><span class="cifra-valor">${semana}</span><span class="cifra-detalle">Fechas en los próximos 7 días</span></div>
    <div class="cifra mal"><span class="cifra-texto">Exámenes pendientes</span><span class="cifra-valor">${futuros.filter((a) => a.tipo === "examen").length}</span><span class="cifra-detalle">Desde hoy en adelante</span></div>
    <div class="cifra info"><span class="cifra-texto">Lo siguiente</span><span class="cifra-valor">${siguiente ? cuando(siguiente.fecha) : "—"}</span><span class="cifra-detalle">${siguiente ? esc(siguiente.titulo) : "No hay fechas próximas"}</span></div>`;

  document.getElementById("lista-proximos").innerHTML = proximos.length
    ? `<ol class="grid gap-3">${proximos.map(tarjeta).join("")}</ol>`
    : htmlVacio("No hay fechas próximas.", tipo ? " Prueba con otro tipo." : " Agrega un aviso con el botón «Nuevo aviso».");
  document.getElementById("contador-pasados").textContent = `(${pasados.length})`;
  document.getElementById("lista-pasados").innerHTML = pasados.length
    ? `<ol class="grid gap-3">${pasados.map(tarjeta).join("")}</ol>`
    : htmlVacio("Nada todavía.", "");
}

/* ---------- Crear y editar ---------- */
function abrir(aviso) {
  editando = aviso ? aviso.id : null;
  document.getElementById("titulo-modal-aviso").textContent = aviso ? "Editar aviso" : "Nuevo aviso";
  modal.showModal();
  // Se llena después de abrir: al cerrarse, app.js limpia el formulario.
  form.titulo.value = aviso ? aviso.titulo : "";
  form.fecha.value = aviso ? aviso.fecha : hoyISO();
  form.tipo.value = aviso ? aviso.tipo : "examen";
  form.curso.value = aviso ? aviso.curso || "" : "";
  form.descripcion.value = aviso ? aviso.descripcion : "";
}

document.getElementById("btn-nuevo").addEventListener("click", () => abrir(null));

form.addEventListener("submit", async (ev) => {
  ev.preventDefault();
  let ok = true;
  form.querySelectorAll("[required]").forEach((c) => {
    const valido = c.checkValidity() && c.value.trim() !== "";
    c.setAttribute("aria-invalid", String(!valido));
    if (!valido) ok = false;
  });
  if (!ok) return toast("Revisa los campos marcados en rojo.", "error");

  const datos = leerFormulario(form);
  const boton = document.getElementById("btn-guardar");
  boton.disabled = true;
  try {
    if (editando === null) await api("/avisos", "POST", datos);
    else await api(`/avisos/${editando}`, "PUT", datos);
    toast(editando === null ? "Aviso creado." : "Aviso actualizado.");
    modal.close();
    await cargar();
  } catch (e) {
    mostrarError(e, "No se pudo guardar el aviso");
  } finally {
    boton.disabled = false;
  }
});

/* ---------- Botones de cada aviso ---------- */
document.querySelector(".contenido").addEventListener("click", async (ev) => {
  const bEditar = ev.target.closest("[data-editar]");
  const bBorrar = ev.target.closest("[data-borrar]");
  if (bEditar) abrir(avisos.find((a) => a.id === Number(bEditar.dataset.editar)));
  if (bBorrar) {
    const aviso = avisos.find((a) => a.id === Number(bBorrar.dataset.borrar));
    if (!aviso || !(await confirmar("Eliminar aviso", `Se eliminará «${aviso.titulo}».`, "Eliminar aviso"))) return;
    try {
      await api(`/avisos/${aviso.id}`, "DELETE");
      toast("Aviso eliminado.");
      await cargar();
    } catch (e) {
      mostrarError(e, "No se pudo eliminar");
    }
  }
});

selFiltro.addEventListener("change", dibujar);

cargar();
