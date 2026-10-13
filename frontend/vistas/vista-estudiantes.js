/* =====================================================================
   Vista Estudiantes (P1)
   ---------------------------------------------------------------------
   Endpoints que usa (formato que debe devolver el C++):

   GET    /api/estudiantes?buscar=ana&orden=nombre
          orden: nombre | nombre-desc | carnet | semestre
          → [ { "carnet": "0901-26-1001", "nombre": "Ana Pérez",
                "carrera": "Ingeniería en Sistemas", "semestre": 2 }, ... ]
   POST   /api/estudiantes            body: { carnet, nombre, carrera, semestre }
   PUT    /api/estudiantes/{carnet}   body: { nombre, carrera, semestre }
   DELETE /api/estudiantes/{carnet}
   Errores: código 4xx con { "error": "mensaje para el usuario" }
   ===================================================================== */

const tablaEl = document.getElementById("tabla-estudiantes");
const contadorEl = document.getElementById("contador");
const buscarEl = document.getElementById("buscar");
const ordenEl = document.getElementById("orden");
const form = document.getElementById("form-estudiante");
const tituloForm = document.getElementById("titulo-form");
const btnGuardar = document.getElementById("btn-guardar");
const inputCarnet = document.getElementById("carnet");
const modal = document.getElementById("modal-estudiante");

let estudiantes = [];
let editando = null; // carnet del estudiante en edición, o null

async function cargarEstudiantes() {
  const params = new URLSearchParams();
  if (buscarEl.value.trim()) params.set("buscar", buscarEl.value.trim());
  params.set("orden", ordenEl.value);
  try {
    estudiantes = await api(`/estudiantes?${params}`);
    dibujarTabla();
  } catch (e) {
    tablaEl.innerHTML = htmlError(`No se pudo cargar la lista: ${e.message}`);
  }
}

function dibujarTabla() {
  contadorEl.textContent = `${estudiantes.length} ${estudiantes.length === 1 ? "estudiante" : "estudiantes"}`;

  if (!estudiantes.length) {
    tablaEl.innerHTML = buscarEl.value.trim()
      ? htmlVacio("Sin resultados.", ` Ningún estudiante coincide con "${buscarEl.value.trim()}".`)
      : htmlVacio("Todavía no hay estudiantes.", " Registra el primero con el formulario.");
    return;
  }

  const filas = estudiantes
    .map(
      (e) => `
      <tr>
        <td>${esc(e.nombre)}<span class="sub">${esc(e.carnet)}</span></td>
        <td>${esc(e.carrera)}</td>
        <td class="numero">${esc(e.semestre)}</td>
        <td class="acciones-fila">
          <button class="btn btn-secundario btn-chico" data-editar="${esc(e.carnet)}" type="button">Editar</button>
          <button class="btn btn-peligro btn-chico" data-eliminar="${esc(e.carnet)}" type="button">Eliminar</button>
        </td>
      </tr>`
    )
    .join("");

  tablaEl.innerHTML = `
    <div class="tabla-contenedor">
      <table class="tabla">
        <thead><tr><th>Estudiante</th><th>Carrera</th><th class="numero">Semestre</th><th><span class="solo-lector">Acciones</span></th></tr></thead>
        <tbody>${filas}</tbody>
      </table>
    </div>`;
}

/* ---------- Formulario (ventana emergente) ---------- */
function modoNuevo() {
  editando = null;
  inputCarnet.disabled = false;
  tituloForm.textContent = "Registrar estudiante";
  btnGuardar.textContent = "Guardar estudiante";
}

function modoEditar(carnet) {
  const e = estudiantes.find((x) => x.carnet === carnet);
  if (!e) return;
  editando = carnet;
  form.carnet.value = e.carnet;
  form.nombre.value = e.nombre;
  form.carrera.value = e.carrera;
  form.semestre.value = e.semestre;
  inputCarnet.disabled = true; // el carnet es la llave, no se edita
  tituloForm.textContent = "Editar estudiante";
  btnGuardar.textContent = "Guardar cambios";
  modal.showModal();
  form.nombre.focus();
}

// app.js limpia el formulario al cerrar; aquí volvemos al modo "nuevo"
modal.addEventListener("close", modoNuevo);

function validar() {
  let valido = true;
  for (const campo of form.querySelectorAll("input[required]")) {
    const ok = campo.disabled || campo.checkValidity();
    campo.setAttribute("aria-invalid", String(!ok));
    if (!ok) valido = false;
  }
  return valido;
}

form.addEventListener("submit", async (ev) => {
  ev.preventDefault();
  if (!validar()) {
    toast("Revisa los campos marcados en rojo.", "error");
    return;
  }
  const datos = leerFormulario(form);
  btnGuardar.disabled = true;
  try {
    if (editando) {
      delete datos.carnet;
      await api(`/estudiantes/${encodeURIComponent(editando)}`, "PUT", datos);
      toast("Cambios guardados.");
    } else {
      await api("/estudiantes", "POST", datos);
      toast(`Estudiante ${datos.nombre} registrado.`);
    }
    modal.close();
    await cargarEstudiantes();
  } catch (e) {
    mostrarError(e, "No se pudo guardar");
  } finally {
    btnGuardar.disabled = false;
  }
});

document.getElementById("btn-nuevo").addEventListener("click", () => {
  modoNuevo();
  modal.showModal();
  inputCarnet.focus();
});

/* ---------- Acciones de la tabla ---------- */
tablaEl.addEventListener("click", async (ev) => {
  const editar = ev.target.closest("[data-editar]");
  const eliminar = ev.target.closest("[data-eliminar]");

  if (editar) modoEditar(editar.dataset.editar);

  if (eliminar) {
    const carnet = eliminar.dataset.eliminar;
    const e = estudiantes.find((x) => x.carnet === carnet);
    const ok = await confirmar(
      "Eliminar estudiante",
      `Se eliminará a ${e ? e.nombre : carnet} junto con sus notas y asistencia.`,
      "Eliminar estudiante"
    );
    if (!ok) return;
    try {
      await api(`/estudiantes/${encodeURIComponent(carnet)}`, "DELETE");
      toast("Estudiante eliminado.");
      await cargarEstudiantes();
    } catch (err) {
      mostrarError(err, "No se pudo eliminar");
    }
  }
});

buscarEl.addEventListener("input", retrasar(cargarEstudiantes, 300));
ordenEl.addEventListener("change", cargarEstudiantes);

cargarEstudiantes();