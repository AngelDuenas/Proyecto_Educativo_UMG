# Cómo darle diseño al manual

El contenido del manual está en [`MANUAL_USUARIO.md`](MANUAL_USUARIO.md). Este archivo explica cómo pasarlo a otra herramienta para que quede con la identidad visual de **CLARD Academy**.

---

## 1. Elegir la herramienta

| Herramienta | Resultado | Cómo pasar el contenido |
|---|---|---|
| **Word** | Documento para imprimir o entregar en PDF | Abre `MANUAL_USUARIO.md` en VS Code, copia todo y pégalo en Word (o conviértelo con Pandoc, ver abajo). Luego aplica los estilos de la sección 3. |
| **Google Docs** | Documento para compartir y editar en equipo | *Archivo → Abrir → Subir* el `.md` (Docs lo convierte), o pega el texto. |
| **Gamma** (gamma.app) | Manual o presentación con diseño automático | *Crear → Pegar texto*, pega el manual completo y el **texto de instrucciones** de la sección 4. |
| **Canva** | Diseño muy visual (folleto o documento) | Crea un "Documento" o "Presentación" y usa *Magic Design* pegando el texto de la sección 4. |
| **Word con Copilot** | Word con diseño asistido | Pega el manual y luego el texto de la sección 4 en Copilot. |

**Convertir a Word automáticamente (opcional)**
Si tienes [Pandoc](https://pandoc.org) instalado, desde la carpeta `docs`:

```
pandoc MANUAL_USUARIO.md -o Manual_CLARD_Academy.docx --toc
```

---

## 2. Material de la marca

**Logos** (carpeta `frontend/img/`):

| Archivo | Uso recomendado |
|---|---|
| `logo-claro.jpg` | Portada sobre fondo blanco |
| `logo-oscuro.jpg` | Portada o encabezados sobre fondo azul marino |
| `logo-blanco.jpg` | Versión con mancha blanca, para fondos oscuros |
| `icono.png` | Ícono pequeño (encabezado o pie de página); fondo transparente |

**Lema:** *Aprende · Crece · Avanza*

**Colores** (tomados del logo):

| Color | Hex | Uso |
|---|---|---|
| Azul marino | `#033857` | Títulos, portada, encabezados de tabla |
| Celeste | `#8EDFFC` | Detalles, recuadros de "consejo", fondos suaves |
| Celeste muy claro | `#E7F9FD` | Fondo de tablas y recuadros |
| Azul "Academy" | `#12ADE0` | Enlaces y resaltados |
| Naranja | `#F7A827` | Recuadros de "importante" y acentos (el triángulo del logo) |
| Tinta | `#062B42` | Texto principal |

**Tipografías** (gratuitas en Google Fonts):
- Títulos: **Outfit** (negrita).
- Texto: **Lexend**.
- Si no están disponibles: *Segoe UI* o *Arial*.

---

## 3. Estilos sugeridos (Word / Google Docs)

| Elemento | Estilo |
|---|---|
| Portada | Fondo azul marino `#033857`, logo `logo-oscuro.jpg` centrado, título en blanco y lema en celeste |
| Título 1 (capítulos) | Outfit 22 pt, azul marino, con una barra naranja `#F7A827` a la izquierda o debajo |
| Título 2 | Outfit 15 pt, azul marino |
| Texto normal | Lexend 11 pt, color tinta `#062B42`, interlineado 1.3 |
| Tablas | Encabezado con fondo celeste muy claro `#E7F9FD` y texto azul marino en negrita; bordes finos grises |
| Recuadro "💡 Consejo" | Fondo `#E7F9FD`, borde izquierdo celeste |
| Recuadro "⚠️ Importante" | Fondo naranja muy suave (`#FEF3DC`), borde izquierdo naranja `#F7A827` |
| Comandos (`.\compilar ejecutar`) | Fuente monoespaciada (Consolas), fondo gris claro |
| Pie de página | "CLARD Academy · Manual de usuario" y número de página |

**Capturas de pantalla:** el manual tiene marcas como *📷 Captura sugerida: …*. Toma esas capturas con el sistema encendido (tecla `Win + Shift + S`), pégalas en ese lugar y borra la marca. Para que se vean uniformes, usa la ventana del navegador maximizada y el mismo zoom en todas.

---

## 4. Texto listo para pegar en Gamma, Canva o Copilot

Copia todo el bloque y pégalo junto con el contenido de `MANUAL_USUARIO.md`:

```
Diseña un manual de usuario profesional a partir del texto que te paso, sin cambiar su contenido
ni el orden de las secciones.

Identidad visual: "CLARD Academy", lema "Aprende · Crece · Avanza".
- Colores: azul marino #033857 (títulos y portada), celeste #8EDFFC (detalles),
  celeste muy claro #E7F9FD (fondos de tablas y recuadros), azul #12ADE0 (resaltados),
  naranja #F7A827 (acentos y avisos importantes), texto #062B42.
- Tipografías: Outfit para títulos y Lexend para el texto.
- Estilo: limpio y moderno, con formas redondeadas suaves (como una mancha celeste)
  y un pequeño triángulo naranja como detalle gráfico.

Estructura:
- Portada con fondo azul marino, el título "Manual de usuario", el nombre CLARD Academy
  y el lema.
- Índice.
- Una sección por cada capítulo del texto, con su número.
- Convierte las tablas en tablas con encabezado celeste claro.
- Convierte los párrafos que empiezan con "💡 Consejo" en recuadros celestes
  y los que empiezan con "⚠️" en recuadros naranjas.
- Donde diga "📷 Captura sugerida", deja un espacio marcado para insertar una imagen,
  con ese texto como descripción.
- Pie de página con "CLARD Academy · Manual de usuario" y el número de página.

Idioma: español. No inventes funciones que no estén en el texto.
```
