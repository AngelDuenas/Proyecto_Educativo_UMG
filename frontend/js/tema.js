/* =====================================================================
   tema.js · Configuración de Tailwind CSS v4 (versión para navegador)
   ---------------------------------------------------------------------
   Identidad visual de CLARD Academy (colores tomados del logo en img/).
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

  /* Marca (logo) */
  --color-marino: #033857;      /* fondo del logo */
  --color-marino-700: #0a4a70;  /* hover sobre marino */
  --color-marino-900: #022a42;
  --color-celeste: #8edffc;     /* mancha del logo */
  --color-celeste-100: #d2f2fd;
  --color-celeste-50: #e7f9fd;  /* fondo del ícono */
  --color-azul: #12ade0;        /* "Academy" */
  --color-azul-700: #0a7fab;    /* azul legible sobre blanco */
  --color-amarillo: #f7a827;    /* triángulo del logo */
  --color-naranja: #f28c1b;
  --color-naranja-700: #c2500a; /* errores y notas reprobadas */

  /* Neutros */
  --color-tinta: #062b42;
  --color-tinta-suave: #4a6a7e;
  --color-papel: #f4fafd;
  --color-cuadricula: #e3f3fa;
  --color-borde: #d5e7ef;
  --color-borde-fuerte: #9bbccb;

  --font-titulo: "Outfit", "Segoe UI", system-ui, sans-serif;
  --font-texto: "Lexend", "Segoe UI", system-ui, sans-serif;
  --font-tiza: "Outfit", "Segoe UI", system-ui, sans-serif;

  --shadow-suave: 0 1px 2px rgb(3 56 87 / 0.06), 0 8px 24px -12px rgb(3 56 87 / 0.25);
  --shadow-alta: 0 2px 4px rgb(3 56 87 / 0.08), 0 18px 40px -16px rgb(3 56 87 / 0.35);
}

@layer base {
  body { @apply bg-papel font-texto text-tinta leading-normal antialiased; }
  h1, h2, h3 { @apply font-titulo font-bold leading-tight tracking-tight text-marino; }
  h1 { @apply text-3xl font-extrabold md:text-4xl; }
  h2 { @apply text-xl; }
  h3 { @apply text-lg; }
  :focus-visible { @apply outline-3 outline-offset-2 outline-amarillo; }
  ::selection { @apply bg-celeste text-marino; }

  input, select { @apply w-full min-w-0 rounded-xl border-2 border-borde bg-white px-3.5 py-2.5 text-tinta transition-colors placeholder:text-tinta-suave/60 hover:border-borde-fuerte; }
  input:focus, select:focus { @apply border-azul outline-none ring-4 ring-celeste/40; }
  select { @apply cursor-pointer appearance-none pr-10 font-medium; }
  input[aria-invalid="true"] { @apply border-naranja-700 bg-naranja-700/5 ring-naranja-700/15; }
}

@layer components {
  /* ---------- Menú lateral (lo dibuja app.js) ---------- */
  .menu { @apply sticky top-0 z-20 flex flex-col bg-marino text-white shadow-alta md:h-screen md:overflow-y-auto md:shadow-none; }
  .menu.abierto { @apply max-h-screen overflow-y-auto; }
  .menu-marca { @apply flex items-center justify-between gap-2 px-4 py-2 md:block md:px-4 md:pt-5 md:pb-2; }
  .menu-marca a { @apply block text-white no-underline; }
  .menu-logo { @apply block h-14 w-auto md:h-auto md:w-full; aspect-ratio: 1.9; object-fit: cover; }
  .menu-lema { @apply hidden text-center text-[0.68rem] tracking-[0.12em] whitespace-nowrap text-celeste/80 uppercase md:block; }
  .menu-boton { @apply cursor-pointer rounded-full border border-celeste/50 px-4 py-1.5 text-sm font-bold text-white hover:bg-white/10 md:hidden; }
  .menu-navegacion { @apply hidden flex-1 px-3 pb-4 md:block; }
  .menu-grupo { @apply mt-5 mb-1.5 px-3 text-[0.7rem] font-bold tracking-[0.18em] text-celeste/70 uppercase; }
  .menu-lista { @apply grid gap-0.5; }
  .menu-enlace { @apply relative flex items-center gap-3 rounded-xl px-3 py-2.5 text-white/85 no-underline transition-colors hover:bg-white/10 hover:text-white; }
  .menu-enlace svg { @apply size-5 flex-none fill-none stroke-current; stroke-width: 1.7; stroke-linecap: round; stroke-linejoin: round; }
  .menu-enlace.activo { @apply bg-celeste font-bold text-marino shadow-suave hover:bg-celeste hover:text-marino; }
  .menu-pie { @apply hidden px-4 py-4 text-sm md:block; }
  .menu.abierto .menu-navegacion, .menu.abierto .menu-pie { @apply block; }
  .estado-servidor { @apply flex items-center gap-2 rounded-xl bg-white/8 px-3 py-2.5 text-[0.82rem] text-celeste; }
  .estado-servidor::before { content: ""; @apply size-2.5 flex-none rounded-full bg-white/40; }
  .estado-servidor[data-estado="conectado"]::before { @apply bg-celeste shadow-[0_0_0_4px_rgb(142_223_252/0.25)]; }
  .estado-servidor[data-estado="demo"]::before { @apply bg-amarillo shadow-[0_0_0_4px_rgb(247_168_39/0.25)]; }

  /* ---------- Paneles y tarjetas ---------- */
  .panel { @apply min-w-0 rounded-2xl border border-borde/70 bg-white p-5 shadow-suave md:p-6; }
  .panel-encabezado { @apply mb-4 flex flex-wrap items-center justify-between gap-3; }
  .panel-encabezado p { @apply text-sm text-tinta-suave; }

  /* Portada: tarjeta azul marino con la mancha celeste del logo */
  .pizarron { @apply relative isolate mb-8 overflow-hidden rounded-3xl bg-marino p-7 text-white shadow-alta md:p-10; }
  .pizarron::before { content: ""; @apply absolute -top-24 -right-20 -z-10 size-80 rounded-[58%_42%_55%_45%/50%_60%_40%_50%] bg-celeste/25; }
  .pizarron::after { content: ""; @apply absolute right-10 bottom-8 -z-10 size-0 border-x-[22px] border-b-[38px] border-x-transparent border-b-amarillo opacity-90; }
  .pizarron h1 { @apply font-titulo text-[clamp(2rem,4.5vw,3rem)] font-extrabold text-white; }
  .pizarron b { @apply mx-0.5 font-titulo text-2xl leading-none font-extrabold text-amarillo; }
  .lema { @apply text-sm font-bold tracking-[0.25em] text-celeste uppercase; }

  .modulo { @apply relative block overflow-hidden rounded-2xl border border-borde/70 bg-white p-5 pl-6 text-tinta no-underline shadow-suave transition duration-200 hover:-translate-y-1 hover:border-celeste hover:shadow-alta; }
  .modulo::before { content: ""; @apply absolute inset-y-0 left-0 w-1.5 bg-celeste transition-colors; }
  .modulo:hover::before { @apply bg-amarillo; }
  .modulo strong { @apply mb-1 block font-titulo text-lg font-bold text-marino; }
  .modulo span { @apply text-sm text-tinta-suave; }
  .lienzo { @apply overflow-auto rounded-2xl border border-borde bg-white; }

  /* ---------- Cifras resumen ---------- */
  .cifra { @apply relative overflow-hidden rounded-2xl border border-borde/70 bg-white px-5 py-4 pl-6 shadow-suave; }
  .cifra::before { content: ""; @apply absolute inset-y-0 left-0 w-1.5 bg-borde-fuerte; }
  .cifra-texto { @apply text-sm font-medium text-tinta-suave; }
  .cifra-valor { @apply my-0.5 block font-titulo text-3xl leading-tight font-extrabold text-marino; }
  .cifra-detalle { @apply text-[0.85rem] text-tinta-suave; }
  .cifra.ok::before { @apply bg-azul; }
  .cifra.ok .cifra-valor { @apply text-azul-700; }
  .cifra.mal::before { @apply bg-naranja-700; }
  .cifra.mal .cifra-valor { @apply text-naranja-700; }
  .cifra.alerta::before { @apply bg-amarillo; }
  .cifra.info::before { @apply bg-marino; }

  /* ---------- Botones ---------- */
  .btn { @apply inline-flex cursor-pointer items-center justify-center gap-2 rounded-full border-2 border-transparent px-5 py-2.5 leading-tight font-bold whitespace-nowrap transition duration-150 active:scale-[0.97] disabled:cursor-not-allowed disabled:opacity-50; }
  .btn-primario { @apply bg-marino text-white shadow-suave hover:bg-marino-700 hover:shadow-alta; }
  .btn-secundario { @apply border-borde bg-white text-marino hover:border-celeste hover:bg-celeste-50; }
  .btn-peligro { @apply border-naranja-700/30 bg-white text-naranja-700 hover:border-naranja-700 hover:bg-naranja-700 hover:text-white; }
  .btn-chico { @apply px-3.5 py-1.5 text-sm; }

  /* ---------- Formularios ---------- */
  .campo { @apply grid content-start gap-1.5; }
  .campo label { @apply text-sm font-bold text-marino; }
  .campo small { @apply text-xs text-tinta-suave; }
  .formulario-botones { @apply mt-1 flex flex-wrap gap-2; }

  /* ---------- Tablas y matrices ---------- */
  .tabla-contenedor { @apply overflow-x-auto rounded-xl border border-borde; }
  .tabla { @apply w-full border-collapse text-[0.95rem] tabular-nums; }
  .tabla th, .tabla td { @apply border-b border-borde/70 px-3.5 py-3 text-left; }
  .tabla th, .tabla .numero, .tabla .acciones-fila, .matriz td { @apply whitespace-nowrap; }
  .tabla thead th { @apply sticky top-0 z-[1] bg-celeste-50 text-xs font-bold tracking-wider text-marino uppercase; }
  .tabla tbody tr { @apply transition-colors; }
  .tabla tbody tr:hover { @apply bg-celeste-50/60; }
  .tabla tbody tr:last-child td { @apply border-b-0; }
  .tabla .numero, .tabla .acciones-fila { @apply text-right; }
  .tabla .acciones-fila .btn + .btn { @apply ml-1.5; }
  .tabla .sub { @apply block text-[0.82rem] text-tinta-suave; }
  .matriz th:first-child, .matriz td:first-child { @apply sticky left-0 z-[2] border-r border-borde bg-white; }
  .matriz thead th:first-child { @apply z-[3] bg-celeste-50; }
  .matriz tbody tr:hover td:first-child { @apply bg-celeste-50; }
  .matriz .celda { @apply px-1.5 py-1.5 text-center; }
  .matriz tfoot td { @apply border-t-2 border-b-0 border-marino bg-celeste-50/50 font-bold; }
  .dimension { @apply rounded-full bg-celeste-50 px-2.5 py-0.5 text-sm text-tinta-suave tabular-nums; }
  .adyacencia td.uno { @apply bg-azul font-bold text-white; }
  .adyacencia td.cero { @apply text-borde-fuerte; }

  /* ---------- Notas por color ---------- */
  .nota-alta { @apply bg-celeste-100 text-marino; }
  .nota-media { @apply bg-amarillo/30 text-tinta; }
  .nota-baja { @apply bg-naranja-700/15 text-naranja-700; }
  .nota-vacia { @apply bg-papel text-tinta-suave; }
  .input-nota { @apply w-14 rounded-lg border-transparent bg-transparent px-1 py-1.5 text-center font-bold hover:border-borde-fuerte focus:bg-white; }

  /* ---------- Etiquetas, leyenda y barras ---------- */
  .etiqueta { @apply inline-block rounded-full bg-borde/70 px-2.5 py-0.5 text-[0.78rem] font-bold text-tinta; }
  .etiqueta.ok { @apply bg-celeste-100 text-azul-700; }
  .etiqueta.mal { @apply bg-naranja-700/12 text-naranja-700; }
  .etiqueta.alerta { @apply bg-amarillo/25 text-tinta; }
  .etiqueta.info { @apply bg-marino text-white; }
  .leyenda { @apply flex flex-wrap gap-4 text-sm text-tinta-suave; }
  .leyenda span { @apply inline-flex items-center gap-1.5; }
  .leyenda i { @apply inline-block size-3.5 rounded-full; }
  .barra { @apply flex min-w-36 items-center gap-2; }
  .barra-pista { @apply h-2.5 flex-1 overflow-hidden rounded-full bg-celeste-50; }
  .barra-relleno { @apply h-full rounded-full bg-azul; }
  .barra-relleno.alerta { @apply bg-amarillo; }
  .barra-relleno.mal { @apply bg-naranja-700; }
  .barra-valor { @apply w-12 text-right font-bold tabular-nums; }

  /* ---------- Pestañas (control segmentado) ---------- */
  .pestanas { @apply mb-5 inline-flex max-w-full gap-1 overflow-x-auto rounded-full border border-borde bg-white p-1 shadow-suave; }
  .pestana { @apply cursor-pointer rounded-full px-4 py-2 text-sm font-bold whitespace-nowrap text-tinta-suave transition-colors hover:text-marino; }
  .pestana[aria-selected="true"] { @apply bg-marino text-white; }

  /* ---------- Asistencia ---------- */
  .marca-asistencia { @apply size-9 cursor-pointer rounded-xl font-bold transition; }
  .marca-asistencia[aria-pressed="true"] { @apply bg-celeste text-marino hover:bg-azul hover:text-white; }
  .marca-asistencia[aria-pressed="false"] { @apply bg-naranja-700/15 text-naranja-700 hover:bg-naranja-700 hover:text-white; }

  /* ---------- Recorridos (árbol y grafo) ---------- */
  .recorrido-lista { @apply flex flex-wrap gap-2; }
  .recorrido-lista li { @apply inline-flex items-center gap-1.5 rounded-full border border-borde bg-white py-1 pr-3 pl-1 text-sm shadow-suave; }
  .recorrido-lista li b { @apply grid size-6 place-items-center rounded-full bg-marino text-xs text-white; }
  .recorrido-lista li.actual { @apply border-amarillo bg-amarillo text-marino; }
  .recorrido-lista li.actual b { @apply bg-white text-marino; }

  /* ---------- Mensajes ---------- */
  .vacio { @apply px-4 py-10 text-center text-tinta-suave; }
  .vacio strong { @apply mb-1 block font-titulo text-lg text-marino; }
  .cargando { @apply p-6 text-center text-tinta-suave; }
  .aviso-demo { @apply mb-6 flex flex-wrap items-center gap-x-3 gap-y-1 rounded-2xl border border-amarillo/50 bg-amarillo/15 px-4 py-3 text-sm text-tinta [overflow-wrap:anywhere]; }
  .aviso-error { @apply rounded-2xl border border-naranja-700/30 bg-naranja-700/8 px-4 py-3 text-tinta; }
  .toasts { @apply fixed inset-auto right-4 bottom-4 m-0 grid max-w-[min(380px,calc(100vw-2rem))] gap-2 overflow-visible border-0 bg-transparent p-0; }
  .toast { @apply rounded-2xl border-l-[6px] border-celeste bg-marino px-4 py-3 text-white shadow-alta; }
  .toast.error { @apply border-naranja; }
  .toast.info { @apply border-amarillo; }
  dialog.confirmacion { @apply m-auto w-[calc(100vw-2rem)] max-w-md rounded-3xl border-0 p-7 text-tinta shadow-alta; }
  dialog.confirmacion p { @apply mt-2 mb-5 text-tinta-suave; }
  dialog.confirmacion .formulario-botones { @apply justify-end; }
  .solo-lector { @apply sr-only; }

  /* ---------- Ventanas emergentes con formulario ---------- */
  .modal { @apply m-auto w-[calc(100vw-2rem)] max-w-lg overflow-hidden rounded-3xl border-0 p-0 text-tinta shadow-alta; }
  .modal-encabezado { @apply flex items-center justify-between gap-3 bg-marino px-6 py-5; }
  .modal-encabezado h2 { @apply text-white; }
  .modal-cerrar { @apply grid size-9 flex-none cursor-pointer place-items-center rounded-full border-0 bg-white/10 p-0 text-white transition-colors duration-150 hover:bg-white/25 active:bg-white/35; }
  .modal-cerrar svg { @apply block size-[18px] fill-none stroke-current; stroke-width: 2.2; stroke-linecap: round; }
  .modal-cuerpo { @apply grid gap-4 p-6; }
  .modal-botones { @apply mt-2 flex flex-wrap justify-end gap-2 border-t border-borde pt-4; }
}
`;

const hojaTailwind = document.createElement("style");
hojaTailwind.type = "text/tailwindcss";
hojaTailwind.textContent = estilosTailwind;
document.head.append(hojaTailwind);
