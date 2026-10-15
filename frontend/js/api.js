/* =====================================================================
   api.js · Conexión entre las vistas y el servidor C++
   ---------------------------------------------------------------------
   Todas las vistas llaman a:   api("/estudiantes", "POST", datos)

   - Respuesta correcta: el JSON que devuelve el servidor.
   - Error: se lanza ApiError con el mensaje {"error": "..."} del C++.
   - Si el servidor no está encendido se lanza un ApiError con estado 0
     y el indicador del menú muestra "Servidor desconectado".
   ===================================================================== */

// Si la página la sirve el propio servidor C++ se usa ruta relativa (/api): así funciona
// en http://127.0.0.1:8080, desde la red local y a través de un túnel (que usa el puerto 443).
// Solo si se abre con doble clic (file://) o con Live Server se apunta al C++ local.
const PUERTOS_LIVE_SERVER = ["5500", "5501"];
const API_BASE =
  location.protocol === "file:" || PUERTOS_LIVE_SERVER.includes(location.port)
    ? "http://127.0.0.1:8080/api"
    : "/api";

const SIN_SERVIDOR = "No hay conexión con el servidor C++. Enciéndelo con: compilar ejecutar";

class ApiError extends Error {
  constructor(estado, mensaje) {
    super(mensaje);
    this.estado = estado;
  }
}

async function api(ruta, metodo = "GET", datos) {
  let respuesta;
  try {
    respuesta = await fetch(API_BASE + ruta, {
      method: metodo,
      headers: datos ? { "Content-Type": "application/json" } : {},
      body: datos ? JSON.stringify(datos) : undefined,
    });
  } catch {
    // TypeError = el servidor no está corriendo (o CORS bloqueado)
    avisarEstado("desconectado");
    throw new ApiError(0, SIN_SERVIDOR);
  }

  // Un servidor que no es el C++ (p. ej. otro servidor de archivos) responde HTML.
  const esJson = (respuesta.headers.get("content-type") || "").includes("application/json");
  if (!esJson && !respuesta.ok) {
    avisarEstado("desconectado");
    throw new ApiError(respuesta.status, SIN_SERVIDOR);
  }

  avisarEstado("conectado");
  const texto = await respuesta.text();
  let cuerpo = null;
  try {
    cuerpo = texto ? JSON.parse(texto) : null;
  } catch {
    cuerpo = texto;
  }

  if (!respuesta.ok) {
    const mensaje = (cuerpo && cuerpo.error) || texto || `Error ${respuesta.status}`;
    throw new ApiError(respuesta.status, mensaje);
  }
  return cuerpo;
}

function avisarEstado(estado) {
  if (window.estadoServidor === estado) return;
  window.estadoServidor = estado;
  document.dispatchEvent(new CustomEvent("servidor-estado", { detail: estado }));
}
