/* =====================================================================
   Vista Inscripciones
   ---------------------------------------------------------------------
   GET    /api/inscripciones          → [ { carnet, nombre, curso, cursoNombre } ]
   GET    /api/inscripciones/{carnet} → { estudiante: {carnet, nombre, semestre},
                                          cursos: [ { codigo, nombre, semestre, creditos, nota,
                                                      estado: "aprobado"|"inscrito"|"disponible"|"bloqueado",
                                                      faltan: [ {codigo, nombre} ] } ],
                                          creditosInscritos }
   POST   /api/inscripciones          body: { carnet, curso }   (400 si le faltan prerrequisitos)
   DELETE /api/inscripciones/{carnet}/{curso}
   ===================================================================== */

const selEstudiante = document.getElementById("estudiante");
const ESTADOS = {
  aprobado: { texto: "Aprobado", clase: "ok" },
  inscrito: { texto: "Inscrito", clase: "info" },
  disponible: { texto: "Disponible", clase: "alerta" },
  bloqueado: { texto: "Bloqueado", clase: "mal" },
};

let detalle = null;

async function cargarEstudiantes() {
  try {
    const estudiantes = await api("/estudiantes?orden=nombre");
    if (!estudiantes.length) {
      selEstudiante.innerHTML = '<option value="">No hay estudiantes</option>';
      document.getElementById("tabla-cursos").innerHTML = htmlVacio("No hay estudiantes.", " Registra uno en la vista Estudiantes.");
      return;
    }
    const pedido = new URLSearchParams(location.search).get("carnet");
    selEstudiante.innerHTML = estudiantes
      .map((e) => `<option value="${esc(e.carnet)}"${e.carnet === pedido ? " selected" : ""}>${esc(e.nombre)} · ${esc(e.carnet)}</option>`)
      .join("");
    await cargarDetalle();
  } catch (e) {
    document.getElementById("tabla-cursos").innerHTML = htmlError(`No se pudieron cargar los estudiantes: ${e.message}`);
  }
  cargarTodas();
}

async function cargarDetalle() {
  const carnet = selEstudiante.value;
  if (!carnet) return;
  try {
    detalle = await api(`/inscripciones/${encodeURIComponent(carnet)}`);
    dibujarCifras();
    dibujarCursos();
  } catch (e) {
    document.getElementById("tabla-cursos").innerHTML = htmlError(`No se pudo cargar al estudiante: ${e.message}`);
  }
}

function dibujarCifras() {
  const contar = (estado) => detalle.cursos.filter((c) => c.estado === estado).length;
  document.getElementById("cifras").innerHTML = `
    <div class="cifra info"><span class="cifra-texto">Inscrito en</span><span class="cifra-valor">${contar("inscrito")}</span><span class="cifra-detalle">${detalle.creditosInscritos} créditos este semestre</span></div>
    <div class="cifra alerta"><span class="cifra-texto">Puede inscribir</span><span class="cifra-valor">${contar("disponible")}</span><span class="cifra-detalle">Prerrequisitos aprobados</span></div>
    <div class="cifra mal"><span class="cifra-texto">Bloqueados</span><span class="cifra-valor">${contar("bloqueado")}</span><span class="cifra-detalle">Le falta aprobar algo antes</span></div>
    <div class="cifra ok"><span class="cifra-texto">Aprobados</span><span class="cifra-valor">${contar("aprobado")}</span><span class="cifra-detalle">Nota de ${CONFIG.NOTA_APROBACION} o más</span></div>`;
}

function dibujarCursos() {
  const el = document.getElementById("tabla-cursos");
  if (!detalle.cursos.length) {
    el.innerHTML = htmlVacio("No hay cursos.", " Registra cursos en Docentes y cursos.");
    return;
  }
  const filas = [...detalle.cursos]
    .sort((a, b) => a.semestre - b.semestre || a.codigo.localeCompare(b.codigo))
    .map((c) => {
      const est = ESTADOS[c.estado] || { texto: c.estado, clase: "" };
      const faltan = c.faltan.length
        ? `<span class="text-sm text-naranja-700">Falta: ${c.faltan.map((f) => esc(f.codigo)).join(", ")}</span>`
        : '<span class="text-sm text-tinta-suave">—</span>';
      let accion = "";
      if (c.estado === "disponible")
        accion = `<button class="btn btn-primario btn-chico" type="button" data-inscribir="${esc(c.codigo)}">Inscribir</button>`;
      else if (c.estado === "inscrito")
        accion = `<button class="btn btn-peligro btn-chico" type="button" data-quitar="${esc(detalle.estudiante.carnet)}|${esc(c.codigo)}">Quitar</button>`;
      return `<tr>
        <td>${esc(c.codigo)}<span class="sub">${esc(c.nombre)}</span></td>
        <td class="numero">${esc(c.semestre)}</td>
        <td class="numero">${esc(c.creditos)}</td>
        <td class="numero">${c.nota === null ? "—" : formatear(c.nota, 0)}</td>
        <td><span class="etiqueta ${est.clase}">${est.texto}</span></td>
        <td>${faltan}</td>
        <td class="acciones-fila">${accion}</td>
      </tr>`;
    })
    .join("");
  el.innerHTML = `
    <div class="tabla-contenedor"><table class="tabla">
      <thead><tr><th>Curso</th><th class="numero">Semestre</th><th class="numero">Créditos</th><th class="numero">Nota</th><th>Estado</th><th>Prerrequisitos pendientes</th><th><span class="solo-lector">Acciones</span></th></tr></thead>
      <tbody>${filas}</tbody>
    </table></div>`;
}

async function cargarTodas() {
  const el = document.getElementById("tabla-todas");
  try {
    const todas = await api("/inscripciones");
    document.getElementById("contador-todas").textContent = `${todas.length} ${todas.length === 1 ? "inscripción" : "inscripciones"}`;
    if (!todas.length) {
      el.innerHTML = htmlVacio("Nadie está inscrito todavía.", " Elige un estudiante arriba e inscríbelo en un curso disponible.");
      return;
    }
    el.innerHTML = `
      <div class="tabla-contenedor"><table class="tabla">
        <thead><tr><th>Estudiante</th><th>Curso</th><th><span class="solo-lector">Acciones</span></th></tr></thead>
        <tbody>${todas
          .map(
            (x) => `<tr>
              <td>${esc(x.nombre)}<span class="sub">${esc(x.carnet)}</span></td>
              <td>${esc(x.curso)}<span class="sub">${esc(x.cursoNombre)}</span></td>
              <td class="acciones-fila"><button class="btn btn-peligro btn-chico" type="button" data-quitar="${esc(x.carnet)}|${esc(x.curso)}">Quitar</button></td>
            </tr>`
          )
          .join("")}</tbody>
      </table></div>`;
  } catch (e) {
    el.innerHTML = htmlError(`No se pudieron cargar las inscripciones: ${e.message}`);
  }
}

selEstudiante.addEventListener("change", cargarDetalle);

document.querySelector(".contenido").addEventListener("click", async (ev) => {
  const bInscribir = ev.target.closest("[data-inscribir]");
  const bQuitar = ev.target.closest("[data-quitar]");
  if (!bInscribir && !bQuitar) return;
  const boton = bInscribir || bQuitar;
  boton.disabled = true;
  try {
    if (bInscribir) {
      const curso = bInscribir.dataset.inscribir;
      await api("/inscripciones", "POST", { carnet: selEstudiante.value, curso });
      toast(`Inscrito en ${curso}.`);
    } else {
      const [carnet, curso] = bQuitar.dataset.quitar.split("|");
      await api(`/inscripciones/${encodeURIComponent(carnet)}/${encodeURIComponent(curso)}`, "DELETE");
      toast(`Inscripción a ${curso} quitada.`);
    }
    await Promise.all([cargarDetalle(), cargarTodas()]);
  } catch (e) {
    mostrarError(e, "No se pudo completar");
    boton.disabled = false;
  }
});

cargarEstudiantes();
