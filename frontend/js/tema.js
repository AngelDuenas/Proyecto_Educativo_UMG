/* =====================================================================
   tema.js · Configuración de Tailwind CSS v4 (versión para navegador)
   ---------------------------------------------------------------------
   Se carga en el <head> de cada página, justo después de Tailwind.
   1. @theme             → paleta y fuentes. Crea clases como bg-marino,
                            text-naranja-700, border-azul, font-titulo...
   2. @layer base        → estilos por defecto (body, títulos, inputs).
   3. @layer components  → piezas que se repiten en varias vistas
                            (.btn, .panel, .tabla, .cifra, .etiqueta...).
   Para cambiar un color en todo el proyecto, se cambia SOLO en @theme.
   ===================================================================== */

const estilosTailwind = String.raw`
@theme static {
  /* Solo se permiten los colores de la paleta del proyecto */
  --color-*: initial;
  --color-white: #ffffff;

  --color-celeste: #9fe7f5;
  --color-celeste-50: #eef9fc;
  --color-azul: #429ebd;
  --color-azul-700: #1d6f8f;
  --color-marino: #053f5c;
  --color-marino-700: #0b5679;
  --color-amarillo: #f7ad19;
  --color-naranja: #f27f0c;
  --color-naranja-700: #c2500a;

  --color-tinta: #0b2b3c;
  --color-tinta-suave: #36566a;
  --color-papel: #eaf4f8;
  --color-cuadricula: #d3e7ef;
  --color-borde: #b9d3de;
  --color-borde-fuerte: #7fa6b7;

  --font-titulo: "Bricolage Grotesque", "Segoe UI", system-ui, sans-serif;
  --font-texto: "Lexend", "Segoe UI", system-ui, sans-serif;
  --font-tiza: "Caveat", "Segoe Print", "Comic Sans MS", cursive;
}

@layer base {
  body { @apply bg-papel font-texto text-tinta leading-normal; }
  h1, h2, h3 { @apply font-titulo font-bold leading-tight tracking-tight text-tinta; }
  h1 { @apply text-3xl font-extrabold; }
  h2 { @apply text-xl; }
  h3 { @apply text-lg; }
  :focus-visible { @apply outline-3 outline-offset-2 outline-amarillo; }

  input, select { @apply w-full min-w-0 rounded-md border-2 border-borde-fuerte bg-white px-3 py-2 text-tinta; }
  input:focus, select:focus { @apply border-marino outline-none; }
  input[aria-invalid="true"] { @apply border-naranja-700 bg-naranja-700/10; }
}

@layer components {
  /* ---------- Menú lateral (lo dibuja app.js) ---------- */
  .menu { @apply sticky top-0 z-20 flex flex-col border-b-[5px] border-naranja bg-marino text-white md:h-screen md:overflow-y-auto md:border-r-[6px] md:border-b-0; }
  .menu.abierto { @apply max-h-screen overflow-y-auto; }
  .menu-marca { @apply flex items-center justify-between gap-2 px-4 py-3 md:px-5 md:pt-6 md:pb-5; }
  .menu-marca a { @apply text-white no-underline; }
  .menu-marca strong { @apply block font-tiza text-4xl leading-none font-bold text-amarillo; }
  .menu-marca span { @apply block text-sm text-celeste; }
  .menu-boton { @apply cursor-pointer rounded-md border border-celeste px-2.5 py-1.5 text-white md:hidden; }
  .menu-navegacion { @apply hidden flex-1 px-3 pb-4 md:block; }
  .menu-grupo { @apply mt-4 mb-1 px-2.5 text-[0.8rem] text-celeste; }
  .menu-enlace { @apply flex items-center gap-3 rounded-md border-l-4 border-transparent px-2.5 py-2 text-white no-underline hover:bg-marino-700; }
  .menu-enlace svg { @apply size-5 flex-none fill-none stroke-current; stroke-width: 1.6; stroke-linecap: round; stroke-linejoin: round; }
  .menu-enlace.activo { @apply border-amarillo bg-marino-700 font-bold; }
  .menu-pie { @apply hidden border-t border-dashed border-celeste/40 px-5 py-4 text-sm text-celeste md:block; }
  .menu.abierto .menu-navegacion, .menu.abierto .menu-pie { @apply block; }
  .estado-servidor { @apply flex items-center gap-2; }
  .estado-servidor::before { content: ""; @apply size-2.5 flex-none rounded-full bg-white/40; }
  .estado-servidor[data-estado="conectado"]::before { @apply bg-celeste; }
  .estado-servidor[data-estado="demo"]::before { @apply bg-amarillo; }

  /* ---------- Paneles y tarjetas ---------- */
  .panel { @apply min-w-0 rounded-xl border border-borde bg-white p-5 shadow-[0_6px_18px_-12px_rgb(5_63_92/0.5)]; }
  .panel-encabezado { @apply mb-4 flex flex-wrap items-center justify-between gap-3; }
  .panel-encabezado p { @apply text-sm text-tinta-suave; }
  .pizarron { @apply mb-6 rounded-xl border-[10px] border-naranja bg-marino p-6 text-white shadow-[inset_0_0_60px_rgb(0_0_0/0.35)] md:p-9; }
  .pizarron h1 { @apply font-tiza text-[clamp(2.4rem,5vw,3.4rem)] text-white; }
  .pizarron b { @apply mx-1 font-tiza text-[1.7rem] leading-none font-bold text-amarillo; }
  .modulo { @apply block rounded-md border-2 border-borde bg-white p-4 text-tinta no-underline hover:border-marino; }
  .modulo strong { @apply mb-1 block font-titulo text-lg font-bold; }
  .modulo span { @apply text-sm text-tinta-suave; }
  .lienzo { @apply overflow-auto rounded-md border border-borde bg-white; }

  /* ---------- Cifras resumen ---------- */
  .cifra { @apply rounded-md border border-t-[6px] border-borde border-t-borde-fuerte bg-white px-4 py-3; }
  .cifra-texto { @apply text-sm text-tinta-suave; }
  .cifra-valor { @apply my-0.5 block font-titulo text-3xl leading-tight font-extrabold; }
  .cifra-detalle { @apply text-[0.85rem] text-tinta-suave; }
  .cifra.ok { @apply border-t-azul-700; }
  .cifra.ok .cifra-valor { @apply text-azul-700; }
  .cifra.mal { @apply border-t-naranja-700; }
  .cifra.mal .cifra-valor { @apply text-naranja-700; }
  .cifra.alerta { @apply border-t-amarillo; }
  .cifra.info { @apply border-t-marino; }

  /* ---------- Botones ---------- */
  .btn { @apply inline-flex cursor-pointer items-center justify-center gap-2 rounded-md border-2 border-transparent px-4 py-2 leading-tight font-bold whitespace-nowrap disabled:cursor-not-allowed disabled:opacity-50; }
  .btn-primario { @apply bg-marino text-white hover:bg-marino-700; }
  .btn-secundario { @apply border-marino bg-white text-marino hover:bg-celeste-50; }
  .btn-peligro { @apply border-naranja-700 bg-white text-naranja-700 hover:bg-naranja-700 hover:text-white; }
  .btn-chico { @apply px-3 py-1 text-sm; }

  /* ---------- Formularios ---------- */
  .campo { @apply grid content-start gap-1; }
  .campo label { @apply text-sm font-bold; }
  .campo small { @apply text-xs text-tinta-suave; }
  .formulario-botones { @apply mt-1 flex flex-wrap gap-2; }

  /* ---------- Tablas y matrices ---------- */
  .tabla-contenedor { @apply overflow-x-auto rounded-md border border-borde; }
  .tabla { @apply w-full border-collapse text-[0.95rem] tabular-nums; }
  .tabla th, .tabla td { @apply border-b border-borde px-3 py-2.5 text-left; }
  .tabla th, .tabla .numero, .tabla .acciones-fila, .matriz td { @apply whitespace-nowrap; }
  .tabla thead th { @apply sticky top-0 z-[1] bg-marino text-sm font-bold text-white; }
  .tabla tbody tr:hover { @apply bg-celeste-50; }
  .tabla tbody tr:last-child td { @apply border-b-0; }
  .tabla .numero, .tabla .acciones-fila { @apply text-right; }
  .tabla .acciones-fila .btn + .btn { @apply ml-1.5; }
  .tabla .sub { @apply block text-[0.82rem] text-tinta-suave; }
  .matriz th:first-child, .matriz td:first-child { @apply sticky left-0 z-[2] border-r border-borde bg-white; }
  .matriz thead th:first-child { @apply z-[3] bg-marino; }
  .matriz tbody tr:hover td:first-child { @apply bg-celeste-50; }
  .matriz .celda { @apply px-1.5 py-1.5 text-center; }
  .matriz tfoot td { @apply border-t-2 border-b-0 border-marino font-bold; }
  .dimension { @apply text-sm text-tinta-suave tabular-nums; }
  .adyacencia td.uno { @apply bg-azul-700 font-bold text-white; }
  .adyacencia td.cero { @apply text-borde-fuerte; }

  /* ---------- Notas por color (sólidos) ---------- */
  .nota-alta { @apply bg-azul-700 text-white; }
  .nota-media { @apply bg-amarillo text-tinta; }
  .nota-baja { @apply bg-naranja-700 text-white; }
  .nota-vacia { @apply bg-celeste-50 text-tinta-suave; }
  .input-nota { @apply w-14 border-transparent px-1 py-1.5 text-center font-bold hover:border-tinta; }

  /* ---------- Etiquetas, leyenda y barras ---------- */
  .etiqueta { @apply inline-block rounded-full bg-borde px-2.5 py-0.5 text-[0.8rem] font-bold text-tinta; }
  .etiqueta.ok { @apply bg-azul-700 text-white; }
  .etiqueta.mal { @apply bg-naranja-700 text-white; }
  .etiqueta.alerta { @apply bg-amarillo text-tinta; }
  .etiqueta.info { @apply bg-marino text-white; }
  .leyenda { @apply flex flex-wrap gap-4 text-sm text-tinta-suave; }
  .leyenda span { @apply inline-flex items-center gap-1.5; }
  .leyenda i { @apply inline-block size-3.5 rounded-sm; }
  .barra { @apply flex min-w-36 items-center gap-2; }
  .barra-pista { @apply h-2.5 flex-1 overflow-hidden rounded-full bg-borde; }
  .barra-relleno { @apply h-full bg-azul-700; }
  .barra-relleno.alerta { @apply bg-amarillo; }
  .barra-relleno.mal { @apply bg-naranja-700; }
  .barra-valor { @apply w-12 text-right font-bold tabular-nums; }

  /* ---------- Pestañas ---------- */
  .pestanas { @apply mb-5 flex gap-1 overflow-x-auto border-b-2 border-borde; }
  .pestana { @apply -mb-0.5 cursor-pointer border-b-4 border-transparent px-4 py-2.5 font-bold whitespace-nowrap text-tinta-suave; }
  .pestana[aria-selected="true"] { @apply border-naranja text-marino; }

  /* ---------- Asistencia ---------- */
  .marca-asistencia { @apply size-9 cursor-pointer rounded-md font-bold text-white; }
  .marca-asistencia[aria-pressed="true"] { @apply bg-azul-700 hover:bg-marino; }
  .marca-asistencia[aria-pressed="false"] { @apply bg-naranja-700 hover:bg-naranja; }

  /* ---------- Recorridos (árbol y grafo) ---------- */
  .recorrido-lista { @apply flex flex-wrap gap-2; }
  .recorrido-lista li { @apply inline-flex items-center gap-1.5 rounded-full border border-borde bg-white py-1 pr-3 pl-1 text-sm; }
  .recorrido-lista li b { @apply grid size-6 place-items-center rounded-full bg-marino text-xs text-white; }
  .recorrido-lista li.actual { @apply border-naranja-700 bg-naranja-700 text-white; }

  /* ---------- Mensajes ---------- */
  .vacio { @apply px-4 py-8 text-center text-tinta-suave; }
  .vacio strong { @apply mb-1 block text-tinta; }
  .cargando { @apply p-6 text-center text-tinta-suave; }
  .aviso-demo { @apply mb-6 flex flex-wrap items-baseline gap-x-3 gap-y-1 rounded-md border-l-[6px] border-naranja bg-amarillo px-4 py-3 text-sm text-tinta; }
  .aviso-error { @apply rounded-md border-l-[6px] border-naranja-700 bg-naranja-700/10 px-4 py-3 text-tinta; }
  .toasts { @apply fixed inset-auto right-4 bottom-4 m-0 grid max-w-[min(380px,calc(100vw-2rem))] gap-2 overflow-visible border-0 bg-transparent p-0; }
  .toast { @apply rounded-md border-l-[6px] border-celeste bg-marino px-4 py-3 text-white shadow-lg; }
  .toast.error { @apply border-naranja; }
  .toast.info { @apply border-amarillo; }
  dialog.confirmacion { @apply m-auto w-[calc(100vw-2rem)] max-w-md rounded-xl border border-borde p-6 text-tinta; }
  dialog.confirmacion p { @apply mt-2 mb-5 text-tinta-suave; }
  dialog.confirmacion .formulario-botones { @apply justify-end; }
  .solo-lector { @apply sr-only; }

  /* ---------- Ventanas emergentes con formulario ---------- */
  .modal { @apply m-auto w-[calc(100vw-2rem)] max-w-lg overflow-hidden rounded-xl p-0 text-tinta shadow-2xl; }
  .modal-encabezado { @apply flex items-center justify-between gap-3 border-b-4 border-naranja bg-marino px-6 py-4; }
  .modal-encabezado h2 { @apply text-white; }
  .modal-cerrar { @apply grid size-9 cursor-pointer place-items-center rounded-md text-2xl leading-none text-white hover:bg-marino-700; }
  .modal-cuerpo { @apply grid gap-4 p-6; }
  .modal-botones { @apply mt-2 flex flex-wrap justify-end gap-2 border-t border-borde pt-4; }
}
`;

const hojaTailwind = document.createElement("style");
hojaTailwind.type = "text/tailwindcss";
hojaTailwind.textContent = estilosTailwind;
document.head.append(hojaTailwind);