/* =====================================================================
   app.js · Lo que comparten TODAS las vistas
   ---------------------------------------------------------------------
   - Dibuja el menú lateral (no hay que copiarlo en cada HTML).
   - Marca la página actual según <body data-pagina="...">.
   - Muestra el estado del servidor.
   - Helpers: toast(), confirmar(), esc(), claseNota(), formatear()...

   Cada HTML solo necesita:
     <body data-pagina="estudiantes" data-raiz="../">
       <div class="app">
         <aside class="menu" id="menu"></aside>
         <main class="contenido"> ... </main>
       </div>
   ===================================================================== */

const CONFIG = {
  NOTA_APROBACION: 61, // nota mínima para aprobar (UMG)
  NOTA_RIESGO: 70, // de 61 a 69 se marca en ámbar
  ASISTENCIA_MINIMA: 80, // % mínimo de asistencia
};

const ICONOS = {
  inicio: '<path d="M3 10l7-6 7 6v7H3z"/><path d="M8 17v-4h4v4"/>',
  estudiantes: '<circle cx="10" cy="7" r="3"/><path d="M4 17c1-3.5 3.5-5 6-5s5 1.5 6 5"/>',
  docentes: '<path d="M3 4h5a2 2 0 012 2v11a2 2 0 00-2-2H3z"/><path d="M17 4h-5a2 2 0 00-2 2v11a2 2 0 012-2h5z"/>',
  calificaciones: '<rect x="3" y="3" width="14" height="14" rx="1"/><path d="M3 8h14M3 12.5h14M8 3v14M12.5 3v14"/>',
  asistencia: '<rect x="3" y="4" width="14" height="13" rx="1"/><path d="M3 8h14M7 2v4M13 2v4M7.5 12.5l2 2 3.5-4"/>',
  reportes: '<path d="M4 17V9M10 17V4M16 17v-6"/>',
  arbol: '<rect x="7.5" y="2" width="5" height="4" rx="1"/><rect x="2" y="14" width="5" height="4" rx="1"/><rect x="13" y="14" width="5" height="4" rx="1"/><path d="M10 6v4M4.5 14v-4h11v4"/>',
  grafo: '<circle cx="4.5" cy="5" r="2"/><circle cx="15.5" cy="5" r="2"/><circle cx="10" cy="15" r="2"/><path d="M6.5 5h7M5.5 6.8l3.5 6.4M14.5 6.8L11 13.2"/>',
  inscripciones: '<path d="M5 3h7l3 3v11H5z"/><path d="M12 3v3h3M8 11l1.5 1.5L12.5 9"/>',
  horario: '<circle cx="10" cy="10" r="7"/><path d="M10 6v4l2.5 2.5"/>',
  avisos: '<path d="M4 8v4h3l5 4V4L7 8z"/><path d="M15 7.5a3.5 3.5 0 010 5"/>',
  algebra: '<path d="M6 3H4v14h2M14 3h2v14h-2"/><path d="M8 7.5h4M8 12.5h4"/>',
  memoria: '<rect x="4" y="4" width="12" height="12" rx="1"/><path d="M8 8h4v4H8zM7 1.5V4M13 1.5V4M7 16v2.5M13 16v2.5M1.5 7H4M1.5 13H4M16 7h2.5M16 13h2.5"/>',
};

const MENU = [
  { grupo: null, items: [{ id: "inicio", texto: "Inicio", href: "index.html" }] },
  {
    grupo: "Gestión",
    items: [
      { id: "estudiantes", texto: "Estudiantes", href: "vistas/vista-estudiantes.html" },
      { id: "docentes", texto: "Docentes y cursos", href: "vistas/vista-docentes-cursos.html" },
      { id: "inscripciones", texto: "Inscripciones", href: "vistas/vista-inscripciones.html" },
      { id: "horario", texto: "Horarios", href: "vistas/vista-horario.html" },
      { id: "avisos", texto: "Avisos y calendario", href: "vistas/vista-avisos.html" },
    ],
  },
  {
    grupo: "Rendimiento",
    items: [
      { id: "calificaciones", texto: "Calificaciones", href: "vistas/vista-calificaciones.html" },
      { id: "asistencia", texto: "Asistencia", href: "vistas/vista-asistencia.html" },
      { id: "reportes", texto: "Reportes", href: "vistas/vista-reportes.html" },
    ],
  },
  {
    // Vistas de muestra: explican cómo aplicamos los temas de los cursos de la universidad.
    grupo: "Temas aplicados",
    items: [
      { id: "arbol", texto: "Estructura académica", href: "vistas/vista-arbol.html" },
      { id: "grafo", texto: "Prerrequisitos", href: "vistas/vista-grafo.html" },
      { id: "algebra", texto: "Álgebra lineal", href: "vistas/vista-algebra.html" },
      { id: "memoria", texto: "Matrices en memoria", href: "vistas/vista-memoria.html" },
    ],
  },
];

/* ---------- Menú lateral ---------- */
function dibujarMenu() {
  const aside = document.getElementById("menu");
  if (!aside) return;
  const raiz = document.body.dataset.raiz || "";
  const actual = document.body.dataset.pagina;

  const grupos = MENU.map(({ grupo, items }) => {
    const enlaces = items
      .map(
        (it) => `
        <li>
          <a class="menu-enlace${it.id === actual ? " activo" : ""}" href="${raiz}${it.href}"
             ${it.id === actual ? 'aria-current="page"' : ""}>
            <svg viewBox="0 0 20 20" aria-hidden="true">${ICONOS[it.id]}</svg>
            ${it.texto}
          </a>
        </li>`
      )
      .join("");
    return `${grupo ? `<p class="menu-grupo">${grupo}</p>` : ""}<ul class="menu-lista">${enlaces}</ul>`;
  }).join("");

  aside.innerHTML = `
    <div class="menu-marca">
      <a href="${raiz}index.html" aria-label="CLARD Academy · Inicio">
        <img class="menu-logo" src="${raiz}img/logo-oscuro.jpg" alt="CLARD Academy">
        <span class="menu-lema">Aprende · Crece · Avanza</span>
      </a>
      <button class="menu-boton" type="button" aria-expanded="false" aria-controls="menu-navegacion">Menú</button>
    </div>
    <nav class="menu-navegacion" id="menu-navegacion" aria-label="Módulos">${grupos}</nav>
    <div class="menu-pie">
      <span class="estado-servidor" data-estado="pendiente">Conectando con el servidor…</span>
    </div>`;

  const boton = aside.querySelector(".menu-boton");
  boton.addEventListener("click", () => {
    const abierto = aside.classList.toggle("abierto");
    boton.setAttribute("aria-expanded", String(abierto));
  });
}

/* ---------- Estado del servidor (indicador en el pie del menú) ---------- */
function actualizarEstado(estado) {
  const marca = document.querySelector(".estado-servidor");
  if (marca) {
    marca.dataset.estado = estado;
    marca.textContent = estado === "conectado" ? "Servidor C++ conectado" : "Servidor desconectado";
  }
}

document.addEventListener("servidor-estado", (e) => actualizarEstado(e.detail));

/* ---------- Avisos flotantes ---------- */
// La pila es un "popover": así se ve encima de las ventanas emergentes.
function toast(mensaje, tipo = "ok") {
  let pila = document.querySelector(".toasts");
  if (!pila) {
    pila = document.createElement("div");
    pila.className = "toasts";
    pila.popover = "manual";
    pila.setAttribute("aria-live", "polite");
    document.body.append(pila);
  }
  const t = document.createElement("div");
  t.className = `toast ${tipo}`;
  t.textContent = mensaje;
  pila.append(t);
  if (pila.matches(":popover-open")) pila.hidePopover();
  pila.showPopover(); // vuelve a ponerla al frente
  setTimeout(() => t.remove(), tipo === "error" ? 6000 : 3200);
}

/* Muestra un error de la API de forma clara */
function mostrarError(error, contexto = "") {
  console.error(error);
  const detalle = error && error.message ? error.message : String(error);
  toast(contexto ? `${contexto}: ${detalle}` : detalle, "error");
}

/* ---------- Confirmación (reemplaza a confirm()) ---------- */
function confirmar(titulo, mensaje, textoBoton = "Eliminar") {
  return new Promise((resolver) => {
    const d = document.createElement("dialog");
    d.className = "confirmacion";
    d.innerHTML = `
      <h2>${esc(titulo)}</h2>
      <p>${esc(mensaje)}</p>
      <div class="formulario-botones">
        <button class="btn btn-secundario" value="no" type="button">Cancelar</button>
        <button class="btn btn-peligro" value="si" type="button">${esc(textoBoton)}</button>
      </div>`;
    document.body.append(d);
    const cerrar = (valor) => {
      d.close();
      d.remove();
      resolver(valor);
    };
    d.querySelector('[value="no"]').addEventListener("click", () => cerrar(false));
    d.querySelector('[value="si"]').addEventListener("click", () => cerrar(true));
    d.addEventListener("cancel", (e) => {
      e.preventDefault();
      cerrar(false);
    });
    d.showModal();
  });
}

/* ---------- Helpers de formato ---------- */

// Escapa texto antes de meterlo en innerHTML (evita romper la página)
function esc(texto) {
  return String(texto ?? "")
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#39;");
}

function formatear(numero, decimales = 1) {
  if (numero === null || numero === undefined || Number.isNaN(numero)) return "—";
  return Number(numero).toLocaleString("es-GT", {
    minimumFractionDigits: decimales,
    maximumFractionDigits: decimales,
  });
}

function claseNota(nota) {
  if (nota === null || nota === undefined || nota === "") return "nota-vacia";
  if (nota < CONFIG.NOTA_APROBACION) return "nota-baja";
  if (nota < CONFIG.NOTA_RIESGO) return "nota-media";
  return "nota-alta";
}

function claseAsistencia(porcentaje) {
  if (porcentaje >= 90) return "";
  if (porcentaje >= CONFIG.ASISTENCIA_MINIMA) return "alerta";
  return "mal";
}

function barra(porcentaje) {
  const p = Math.max(0, Math.min(100, Number(porcentaje) || 0));
  return `
    <div class="barra" title="${formatear(p)} %">
      <div class="barra-pista"><div class="barra-relleno ${claseAsistencia(p)}" style="width:${p}%"></div></div>
      <span class="barra-valor">${formatear(p, 0)}%</span>
    </div>`;
}

function htmlVacio(titulo, texto) {
  return `<div class="vacio"><strong>${esc(titulo)}</strong>${esc(texto)}</div>`;
}

function htmlError(texto) {
  return `<div class="aviso-error" role="alert">${esc(texto)}</div>`;
}

// Espera a que el usuario deje de escribir antes de ejecutar
function retrasar(fn, ms = 300) {
  let t;
  return (...args) => {
    clearTimeout(t);
    t = setTimeout(() => fn(...args), ms);
  };
}

// Lee un <form> como objeto { nombreCampo: valor }
function leerFormulario(form) {
  const datos = Object.fromEntries(new FormData(form).entries());
  for (const campo of form.querySelectorAll('input[type="number"]')) {
    if (campo.name in datos) datos[campo.name] = datos[campo.name] === "" ? null : Number(datos[campo.name]);
  }
  for (const k of Object.keys(datos)) if (typeof datos[k] === "string") datos[k] = datos[k].trim();
  return datos;
}

/* ---------- Ventanas emergentes (formularios) ---------- */
// <button data-abrir="modal-x">   abre   <dialog class="modal" id="modal-x">
// <button data-cerrar>            cierra la ventana donde está
// Clic fuera de la ventana también la cierra.
document.addEventListener("click", (ev) => {
  const abrir = ev.target.closest("[data-abrir]");
  if (abrir) document.getElementById(abrir.dataset.abrir).showModal();
  if (ev.target.closest("[data-cerrar]")) ev.target.closest("dialog").close();
  if (ev.target.matches("dialog.modal")) ev.target.close();
});

// Al cerrar una ventana, su formulario queda limpio para la próxima vez
document.querySelectorAll("dialog.modal").forEach((modal) =>
  modal.addEventListener("close", () => {
    const form = modal.querySelector("form");
    if (!form) return;
    form.reset();
    form.querySelectorAll("[aria-invalid]").forEach((c) => c.removeAttribute("aria-invalid"));
  })
);

/* ---------- Tablas en celular ---------- */
// En pantallas chicas cada fila de una tabla se muestra como tarjeta (ver estilos.css).
// Aquí se copia el nombre de cada columna a sus celdas (data-label) para poder mostrarlo.
// Las matrices (.matriz) y el horario no se tocan: esas se desplazan hacia los lados.
function prepararTablas() {
  document.querySelectorAll("table.tabla:not(.matriz):not(.horario)").forEach((tabla) => {
    const titulos = [...tabla.querySelectorAll("thead th")].map((th) =>
      th.querySelector(".solo-lector") ? "" : th.textContent.trim()
    );
    tabla.classList.add("tabla-movil");
    // En los rankings la 1.ª columna es el puesto: el título de la tarjeta pasa a ser la 2.ª
    tabla.classList.toggle("tabla-ranking", titulos[0] === "Puesto");
    tabla.querySelectorAll("tbody tr").forEach((tr) =>
      [...tr.children].forEach((td, i) => {
        if (!td.hasAttribute("data-label")) td.setAttribute("data-label", titulos[i] || "");
      })
    );
  });
}

// Las vistas redibujan sus tablas con innerHTML: se vuelven a preparar en el siguiente cuadro.
let tablasPendientes = false;
new MutationObserver(() => {
  if (tablasPendientes) return;
  tablasPendientes = true;
  requestAnimationFrame(() => {
    tablasPendientes = false;
    prepararTablas();
  });
}).observe(document.body, { childList: true, subtree: true });

/* ---------- Arranque ---------- */
dibujarMenu();
if (window.estadoServidor) actualizarEstado(window.estadoServidor);