/* =====================================================================
   Vista Docentes y Cursos (P5)
   ---------------------------------------------------------------------
   GET    /api/docentes   → [ { "id": "D01", "nombre": "...", "especialidad": "..." } ]
   POST   /api/docentes   body: { nombre, especialidad }  (el C++ genera el id)
   DELETE /api/docentes/{id}

   GET    /api/cursos     → [ { "codigo": "MAT101", "nombre": "...", "creditos": 5, "semestre": 1 } ]
   POST   /api/cursos     body: { codigo, nombre, creditos, semestre }
   DELETE /api/cursos/{codigo}

   GET    /api/asignaciones → [ { "docenteId": "D01", "cursoCodigo": "MAT101" } ]
   POST   /api/asignaciones body: { docenteId, cursoCodigo }
   DELETE /api/asignaciones/{docenteId}/{cursoCodigo}
   ===================================================================== */

let docentes = [];
let cursos = [];
let asignaciones = [];

/* ---------- Pestañas ---------- */
const pestanas = document.querySelectorAll(".pestana");
pestanas.forEach((tab) =>
  tab.addEventListener("click", () => {
    pestanas.forEach((t) => {
      const activa = t === tab;
      t.setAttribute("aria-selected", String(activa));
      document.getElementById(t.getAttribute("aria-controls")).hidden = !activa;
    });
  })
);

/* ---------- Carga ---------- */
async function cargarTodo() {
  try {
    [docentes, cursos, asignaciones] = await Promise.all([api("/docentes"), api("/cursos"), api("/asignaciones")]);
    dibujarDocentes();
    dibujarCursos();
    dibujarAsignaciones();
    llenarSelects();
  } catch (e) {
    const msg = htmlError(`No se pudieron cargar los datos: ${e.message}`);
    ["tabla-docentes", "tabla-cursos", "tabla-asignaciones"].forEach((id) => (document.getElementById(id).innerHTML = msg));
  }
}

const nombreDocente = (id) => (docentes.find((d) => d.id === id) || {}).nombre || id;
const nombreCurso = (codigo) => (cursos.find((c) => c.codigo === codigo) || {}).nombre || codigo;

function contar(id, n, singular, plural) {
  document.getElementById(id).textContent = `${n} ${n === 1 ? singular : plural}`;
}

function dibujarDocentes() {
  contar("contador-docentes", docentes.length, "docente", "docentes");
  const el = document.getElementById("tabla-docentes");
  if (!docentes.length) {
    el.innerHTML = htmlVacio("No hay docentes.", " Registra uno con el formulario.");
    return;
  }
  el.innerHTML = `
    <div class="tabla-contenedor"><table class="tabla">
      <thead><tr><th>Código</th><th>Nombre</th><th>Especialidad</th><th class="numero">Cursos</th><th><span class="solo-lector">Acciones</span></th></tr></thead>
      <tbody>${docentes
        .map((d) => {
          const n = asignaciones.filter((a) => a.docenteId === d.id).length;
          return `<tr>
            <td>${esc(d.id)}</td><td>${esc(d.nombre)}</td><td>${esc(d.especialidad || "—")}</td>
            <td class="numero">${n}</td>
            <td class="acciones-fila"><button class="btn btn-peligro btn-chico" type="button" data-borrar-docente="${esc(d.id)}">Eliminar</button></td>
          </tr>`;
        })
        .join("")}</tbody>
    </table></div>`;
}

function dibujarCursos() {
  contar("contador-cursos", cursos.length, "curso", "cursos");
  const el = document.getElementById("tabla-cursos");
  if (!cursos.length) {
    el.innerHTML = htmlVacio("No hay cursos.", " Registra uno con el formulario.");
    return;
  }
  const ordenados = [...cursos].sort((a, b) => a.semestre - b.semestre || a.codigo.localeCompare(b.codigo));
  el.innerHTML = `
    <div class="tabla-contenedor"><table class="tabla">
      <thead><tr><th>Código</th><th>Curso</th><th class="numero">Créditos</th><th class="numero">Semestre</th><th>Docente</th><th><span class="solo-lector">Acciones</span></th></tr></thead>
      <tbody>${ordenados
        .map((c) => {
          const asig = asignaciones.filter((a) => a.cursoCodigo === c.codigo);
          const docente = asig.length
            ? asig.map((a) => esc(nombreDocente(a.docenteId))).join(", ")
            : '<span class="etiqueta alerta">Sin docente</span>';
          return `<tr>
            <td>${esc(c.codigo)}</td><td>${esc(c.nombre)}</td>
            <td class="numero">${esc(c.creditos)}</td><td class="numero">${esc(c.semestre)}</td>
            <td>${docente}</td>
            <td class="acciones-fila"><button class="btn btn-peligro btn-chico" type="button" data-borrar-curso="${esc(c.codigo)}">Eliminar</button></td>
          </tr>`;
        })
        .join("")}</tbody>
    </table></div>`;
}

function dibujarAsignaciones() {
  contar("contador-asignaciones", asignaciones.length, "asignación", "asignaciones");
  const el = document.getElementById("tabla-asignaciones");
  if (!asignaciones.length) {
    el.innerHTML = htmlVacio("No hay asignaciones.", " Elige un docente y un curso en el formulario.");
    return;
  }
  el.innerHTML = `
    <div class="tabla-contenedor"><table class="tabla">
      <thead><tr><th>Docente</th><th>Curso</th><th><span class="solo-lector">Acciones</span></th></tr></thead>
      <tbody>${asignaciones
        .map(
          (a) => `<tr>
            <td>${esc(nombreDocente(a.docenteId))}</td>
            <td>${esc(a.cursoCodigo)}<span class="sub">${esc(nombreCurso(a.cursoCodigo))}</span></td>
            <td class="acciones-fila"><button class="btn btn-peligro btn-chico" type="button"
                data-quitar="${esc(a.docenteId)}|${esc(a.cursoCodigo)}">Quitar</button></td>
          </tr>`
        )
        .join("")}</tbody>
    </table></div>`;
}

function llenarSelects() {
  const selDoc = document.getElementById("asig-docente");
  const selCur = document.getElementById("asig-curso");
  selDoc.innerHTML = docentes.length
    ? docentes.map((d) => `<option value="${esc(d.id)}">${esc(d.nombre)}</option>`).join("")
    : '<option value="">Primero registra un docente</option>';
  selCur.innerHTML = cursos.length
    ? cursos.map((c) => `<option value="${esc(c.codigo)}">${esc(c.codigo)}  ${esc(c.nombre)}</option>`).join("")
    : '<option value="">Primero registra un curso</option>';
}

/* ---------- Formularios ---------- */
function validar(form) {
  let ok = true;
  form.querySelectorAll("input[required], select[required]").forEach((c) => {
    const valido = c.checkValidity() && c.value !== "";
    c.setAttribute("aria-invalid", String(!valido));
    if (!valido) ok = false;
  });
  if (!ok) toast("Revisa los campos marcados en rojo.", "error");
  return ok;
}

async function enviar(form, ruta, mensaje, transformar = (d) => d) {
  if (!validar(form)) return;
  const boton = form.querySelector('[type="submit"]');
  boton.disabled = true;
  try {
    const datos = transformar(leerFormulario(form));
    await api(ruta, "POST", datos);
    toast(mensaje(datos));
    form.closest("dialog").close(); // al cerrar se limpia el formulario
    await cargarTodo();
  } catch (e) {
    mostrarError(e, "No se pudo guardar");
  } finally {
    boton.disabled = false;
  }
}

document.getElementById("form-docente").addEventListener("submit", (ev) => {
  ev.preventDefault();
  enviar(ev.target, "/docentes", (d) => `Docente ${d.nombre} registrado.`);
});

document.getElementById("form-curso").addEventListener("submit", (ev) => {
  ev.preventDefault();
  enviar(ev.target, "/cursos", (d) => `Curso ${d.codigo} registrado.`, (d) => ({ ...d, codigo: d.codigo.toUpperCase() }));
});

document.getElementById("form-asignacion").addEventListener("submit", (ev) => {
  ev.preventDefault();
  enviar(ev.target, "/asignaciones", (d) => `${nombreCurso(d.cursoCodigo)} asignado a ${nombreDocente(d.docenteId)}.`);
});

/* ---------- Eliminar ---------- */
document.querySelector(".contenido").addEventListener("click", async (ev) => {
  const bDoc = ev.target.closest("[data-borrar-docente]");
  const bCur = ev.target.closest("[data-borrar-curso]");
  const bAsig = ev.target.closest("[data-quitar]");
  let ruta = null;
  let ok = false;

  if (bDoc) {
    const id = bDoc.dataset.borrarDocente;
    ok = await confirmar("Eliminar docente", `Se eliminará a ${nombreDocente(id)} y sus asignaciones.`, "Eliminar docente");
    ruta = `/docentes/${encodeURIComponent(id)}`;
  } else if (bCur) {
    const codigo = bCur.dataset.borrarCurso;
    ok = await confirmar("Eliminar curso", `Se eliminará ${codigo} (${nombreCurso(codigo)}), sus asignaciones y prerrequisitos.`, "Eliminar curso");
    ruta = `/cursos/${encodeURIComponent(codigo)}`;
  } else if (bAsig) {
    const [docenteId, curso] = bAsig.dataset.quitar.split("|");
    ok = true; // quitar una asignación es fácil de deshacer
    ruta = `/asignaciones/${encodeURIComponent(docenteId)}/${encodeURIComponent(curso)}`;
  }

  if (!ruta || !ok) return;
  try {
    await api(ruta, "DELETE");
    toast(bAsig ? "Asignación quitada." : "Eliminado.");
    await cargarTodo();
  } catch (e) {
    mostrarError(e, "No se pudo eliminar");
  }
});

cargarTodo();