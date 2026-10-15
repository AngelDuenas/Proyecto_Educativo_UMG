# Manual de usuario · CLARD Academy

**Sistema de gestión escolar** · *Aprende · Crece · Avanza*

Este manual explica cómo encender el sistema y cómo usar cada una de sus pantallas: registrar estudiantes, docentes y cursos, inscribir, armar horarios, publicar avisos, poner notas, tomar asistencia y consultar reportes. Al final están las vistas de muestra, que explican cómo se aplican en el sistema los temas de los cursos de la universidad.

---

## Contenido

1. [Antes de empezar](#1-antes-de-empezar)
2. [Encender el sistema](#2-encender-el-sistema)
3. [Conocer la pantalla](#3-conocer-la-pantalla)
4. [Inicio](#4-inicio)
5. [Estudiantes](#5-estudiantes)
6. [Docentes y cursos](#6-docentes-y-cursos)
7. [Inscripciones](#7-inscripciones)
8. [Horarios](#8-horarios)
9. [Avisos y calendario](#9-avisos-y-calendario)
10. [Calificaciones](#10-calificaciones)
11. [Asistencia](#11-asistencia)
12. [Reportes](#12-reportes)
13. [Temas aplicados (vistas de muestra)](#13-temas-aplicados-vistas-de-muestra)
14. [Cómo se guardan los datos](#14-cómo-se-guardan-los-datos)
15. [Usar el sistema desde otro dispositivo](#15-usar-el-sistema-desde-otro-dispositivo)
16. [Solución de problemas](#16-solución-de-problemas)
17. [Glosario](#17-glosario)

---

## 1. Antes de empezar

**Qué necesitas**

| Requisito | Detalle |
|---|---|
| Computadora | Windows 10 u 11 |
| Compilador | Visual Studio 2022 con la carga **"Desarrollo para el escritorio con C++"** |
| Navegador | Chrome, Edge o Firefox actualizados |
| Proyecto | La carpeta `Proyecto_Educativo_UMG` descargada del repositorio |

No hace falta instalar ninguna base de datos: el sistema guarda la información en archivos dentro de la carpeta `data/`.

**Reglas que conviene conocer**

- Una nota **aprueba con 61 o más** (de 0 a 100).
- El **carnet** identifica a cada estudiante y no se puede repetir (formato `0901-26-1009`).
- El **código** identifica a cada curso y no se puede repetir (formato `MAT101`: tres letras y tres números).
- La semana de clases va de **lunes a viernes**.

---

## 2. Encender el sistema

1. Abre la carpeta del proyecto en **Visual Studio Code**.
2. Abre la terminal: menú **Terminal → Nueva terminal** (o `Ctrl + ñ`).
3. Escribe y presiona Enter:

   ```
   .\compilar ejecutar
   ```

4. Espera el mensaje **`Servidor listo en http://127.0.0.1:8080`**.
5. Abre el navegador en **http://127.0.0.1:8080**.

> 📷 *Captura sugerida: la terminal con el mensaje "Servidor listo".*

**Para apagarlo**, vuelve a la terminal y presiona `Ctrl + C`.

> 💡 **Consejo:** usa `127.0.0.1` en lugar de `localhost`. En Windows, `localhost` puede tardar unos segundos en cada carga.

**Otras formas de encenderlo**

| Quiero… | Escribo |
|---|---|
| Solo compilar, sin encender | `.\compilar` |
| Compilar y encender | `.\compilar ejecutar` |
| Compartirlo por internet | `.\tunel` (ver [sección 15](#15-usar-el-sistema-desde-otro-dispositivo)) |

También puedes hacer **doble clic** en `compilar.cmd` desde el explorador de archivos.

---

## 3. Conocer la pantalla

> 📷 *Captura sugerida: pantalla de Inicio completa, señalando el menú, el indicador del servidor y el contenido.*

**Menú lateral (izquierda)**
Tiene el logo y los módulos agrupados en tres secciones:

| Grupo | Módulos |
|---|---|
| **Gestión** | Estudiantes · Docentes y cursos · Inscripciones · Horarios · Avisos y calendario |
| **Rendimiento** | Calificaciones · Asistencia · Reportes |
| **Temas aplicados** | Estructura académica · Prerrequisitos · Álgebra lineal |

La opción en la que estás aparece resaltada en celeste.

**Indicador del servidor (abajo del menú)**

| Indicador | Significa |
|---|---|
| 🔵 **Servidor C++ conectado** | Todo funciona normal y los cambios se guardan. |
| 🟡 **Servidor desconectado** | El servidor está apagado: las pantallas no cargan datos y aparece un aviso de error. Enciende el sistema (sección 2). |

**Elementos comunes**

- **Tarjetas de resumen:** cifras importantes arriba de cada pantalla (por ejemplo, el promedio general).
- **Ventanas emergentes:** los formularios se abren encima de la pantalla. Se cierran con **Cancelar**, con la **×** o haciendo clic fuera de la ventana.
- **Avisos flotantes:** abajo a la derecha aparece un mensaje que confirma cada acción (o explica qué salió mal).
- **Confirmación al eliminar:** antes de borrar algo importante, el sistema pregunta si estás seguro.

**En celular o tablet**
El menú se esconde arriba; tócalo con el botón **Menú**. Las tablas se muestran como tarjetas y el horario se ve un día a la vez.

---

## 4. Inicio

> 📷 *Captura sugerida: Inicio con la tarjeta de bienvenida y las próximas fechas.*

Es la pantalla principal. Muestra:

- **Resumen del sistema:** cuántos estudiantes, cursos y docentes hay, el promedio general de notas y la asistencia de la semana.
- **Próximas fechas:** los 3 avisos más cercanos (exámenes, entregas o eventos). El botón **Ver calendario** lleva a Avisos.
- **Accesos a cada módulo**, organizados igual que el menú.

---

## 5. Estudiantes

> 📷 *Captura sugerida: lista de estudiantes con la búsqueda.*

### Registrar un estudiante
1. Presiona **Registrar estudiante**.
2. Llena el formulario:
   - **Carnet** (obligatorio, formato `0901-26-1009`).
   - **Semestre** (de 1 a 10).
   - **Nombre completo** (obligatorio).
   - **Carrera** (puedes elegir una de la lista o escribirla).
3. Presiona **Guardar estudiante**.

Si el carnet ya existe, el sistema avisa y no lo registra.

### Buscar y ordenar
- Escribe en **Buscar por nombre o carnet**: la lista se filtra mientras escribes (no importan las tildes ni las mayúsculas).
- Usa **Ordenar por** para ver la lista por nombre (A a Z o Z a A), por carnet o por semestre.

### Editar
Presiona **Editar** en la fila del estudiante, cambia los datos y guarda. El carnet no se puede cambiar.

### Eliminar
Presiona **Eliminar** y confirma.
> ⚠️ Al eliminar un estudiante también se borran **sus notas, su asistencia y sus inscripciones**.

---

## 6. Docentes y cursos

Esta pantalla tiene tres pestañas: **Docentes**, **Cursos** y **Asignaciones**.

> 📷 *Captura sugerida: las tres pestañas.*

### Docentes
1. Presiona **Registrar docente**.
2. Escribe el **nombre completo** (obligatorio) y la **especialidad**.
3. Presiona **Guardar docente**. El sistema le asigna un código (`D01`, `D02`…).

La columna **Cursos** indica cuántos cursos imparte cada docente.

### Cursos
1. Presiona **Registrar curso**.
2. Llena **Código** (`MAT101`), **Créditos**, **Nombre del curso** y **Semestre**.
3. Presiona **Guardar curso**.

Los cursos sin docente aparecen con la etiqueta **Sin docente**. El sistema admite hasta 15 cursos.

> ⚠️ Al eliminar un curso se borran sus notas y se quita de las asignaciones, los prerrequisitos, el horario y las inscripciones. Los avisos de ese curso pasan a ser generales.

### Asignaciones (qué docente da qué curso)
1. Presiona **Asignar curso**.
2. Elige el **docente** y el **curso**.
3. Presiona **Asignar curso**.

Para quitar una asignación usa **Quitar**. No se puede asignar dos veces el mismo curso al mismo docente.

---

## 7. Inscripciones

> 📷 *Captura sugerida: un estudiante con cursos en los cuatro estados.*

Sirve para inscribir a cada estudiante en los cursos que **puede** llevar. El sistema revisa los prerrequisitos automáticamente.

1. Elige al **estudiante** en la parte superior.
2. Revisa la tabla **Cursos del plan**. Cada curso tiene un estado:

| Estado | Significa | Acción |
|---|---|---|
| 🔵 **Aprobado** | Ya lo aprobó (nota de 61 o más). | — |
| ⚫ **Inscrito** | Lo está llevando este semestre. | **Quitar** |
| 🟡 **Disponible** | Aprobó todos sus prerrequisitos. | **Inscribir** |
| 🟠 **Bloqueado** | Le falta aprobar algún curso anterior; la columna *Prerrequisitos pendientes* dice cuál. | — |

3. Presiona **Inscribir** en un curso disponible.

Las tarjetas de arriba resumen cuántos cursos lleva, cuántos créditos suma y cuántos tiene disponibles o bloqueados. Abajo, **Todas las inscripciones** muestra la lista completa de todos los estudiantes.

---

## 8. Horarios

> 📷 *Captura sugerida: horario semanal con un choque marcado.*

El horario es una cuadrícula de **5 días × 6 periodos** (de 07:00 a 12:20, con receso de 09:30 a 09:50). Cada curso tiene un color según su semestre.

### Editar un bloque
1. Haz clic en el bloque (día y hora) que quieres cambiar.
2. Marca los cursos que se dan en ese bloque (**hasta 3**).
3. Presiona **Guardar bloque**.

### Choques
Un **choque** ocurre cuando un docente o un estudiante tiene dos cursos a la misma hora. El sistema:
- Guarda el bloque igual, pero avisa del choque.
- Marca el bloque en rojo con la etiqueta **Choque**.
- Lo explica en la lista **Choques de horario** (quién y qué cursos).

### Horario de un estudiante
En **Ver horario de**, elige a un estudiante para ver solo los cursos en los que está inscrito. En esta vista no se edita; vuelve a **Todos los cursos** para hacer cambios.

### Bloques por curso
Compara cuántos bloques tiene cada curso a la semana con sus créditos, y marca los cursos **Sin horario**.

> 📱 En celular y tablet se ve **un día a la vez**: elige el día con los botones Lun–Vie.

---

## 9. Avisos y calendario

> 📷 *Captura sugerida: lista de próximas fechas.*

Reúne exámenes, entregas, eventos y avisos generales, ordenados por fecha.

### Crear un aviso
1. Presiona **Nuevo aviso**.
2. Llena:
   - **Título** (obligatorio).
   - **Fecha** (obligatoria).
   - **Tipo**: Examen, Entrega, Evento o Aviso general.
   - **Curso relacionado** (opcional; déjalo en *General* si aplica a todos).
   - **Descripción** (temas, lugar, qué llevar…).
3. Presiona **Guardar aviso**.

### Leer el calendario
- Las fechas de **los próximos 7 días** aparecen resaltadas en amarillo.
- Cada aviso indica cuánto falta: *Hoy*, *Mañana*, *En 6 días*…
- Filtra por tipo con el selector **Todos los tipos**.
- Las fechas que ya pasaron están al final, en **Fechas que ya pasaron** (haz clic para desplegarlas).

Para cambiar o borrar un aviso usa **Editar** o **Eliminar**.

---

## 10. Calificaciones

> 📷 *Captura sugerida: matriz de notas con colores.*

Las notas se muestran en una tabla: **cada fila es un estudiante y cada columna un curso**.

### Poner o cambiar una nota
1. Haz clic en la celda.
2. Escribe la nota (de **0 a 100**).
3. Sal de la celda (clic fuera o tecla Tab). **Se guarda sola.**

Para borrar una nota, deja la celda vacía. Si escribes un valor fuera de 0 a 100, aparece un aviso y la celda vuelve a su valor anterior.

### Colores

| Color | Rango |
|---|---|
| Celeste | 70 a 100 |
| Ámbar | 61 a 69 (aprobado, pero en riesgo) |
| Rojizo | Menos de 61 (reprobado) |

Arriba están las estadísticas: **promedio general**, **nota más alta**, **nota más baja** y **aprobados / reprobados**. La última columna muestra el promedio de cada estudiante y la última fila el promedio de cada curso.

---

## 11. Asistencia

> 📷 *Captura sugerida: matriz de asistencia.*

La asistencia es una tabla de **estudiantes × días (L a V)**.

- Haz clic en un día para cambiar entre **presente (✓)** y **ausente (✗)**. Se guarda al momento.
- La última columna muestra el **porcentaje de asistencia** de cada estudiante.
- La última fila muestra la **asistencia de cada día**.

Las tarjetas de arriba indican la asistencia general, el día con más ausencias y cuántos estudiantes están **bajo 80 %**.

---

## 12. Reportes

> 📷 *Captura sugerida: los dos rankings.*

- **Ranking por promedio:** estudiantes ordenados por su promedio. Puedes ver *mejores primero* o *más bajos primero*.
- **Ranking por asistencia:** estudiantes ordenados por su porcentaje de asistencia.
- **Cursos relacionados:** elige un curso y verás, en orden de cercanía, los cursos que dependen de él.
- **Imprimir reporte:** prepara la página para imprimir o guardar como PDF (desde el diálogo de impresión elige *Guardar como PDF*).

---

## 13. Temas aplicados (vistas de muestra)

Estas tres pantallas **no se editan**: sirven para explicar cómo se aplican en el sistema los temas de los cursos de la universidad, usando los datos reales.

### Estructura académica (árboles)
> 📷 *Captura sugerida: el árbol con un recorrido animado.*

Muestra la universidad como un **árbol**: Universidad → Facultad → Carrera → Semestres → Cursos. Las tarjetas indican el número de nodos, de hojas (cursos) y la altura del árbol.

1. Elige el recorrido: **Preorden**, **Postorden** o **Por niveles**.
2. Presiona **Animar recorrido** para ver, paso a paso, el orden en que se visitan los nodos.

### Prerrequisitos (grafos)
> 📷 *Captura sugerida: el grafo con un recorrido BFS.*

Muestra los cursos como un **grafo dirigido**: una flecha de A hacia B significa que **A es prerrequisito de B**.

1. Elige un **curso de origen**.
2. Presiona **Recorrer en anchura (BFS)** o **Recorrer en profundidad (DFS)** para ver el orden de visita.

Abajo está la **matriz de adyacencia**: una celda vale 1 cuando el curso de la fila es prerrequisito del de la columna.

### Álgebra lineal
> 📷 *Captura sugerida: la sección de producto matriz × vector.*

Explica seis operaciones, cada una con su fórmula y el resultado calculado:

| # | Operación | Para qué se usa en el sistema |
|---|---|---|
| 1 | Matriz de notas **A** | Guardar las notas (estudiantes × cursos). |
| 2 | Transpuesta **Aᵀ** | Trabajar las notas "por curso". |
| 3 | Producto **A · c** | Calcular el promedio ponderado por créditos. |
| 4 | **(1/n) · Aᵀ · 1** | Calcular el promedio de cada curso. |
| 5 | Combinación lineal **0.9 · p + 0.1 · a** | Un indicador que combina notas y asistencia. |
| 6 | Potencia **M²** | Contar las cadenas de prerrequisitos de dos pasos. |

Usa los botones de arriba para saltar a cada sección.

### Matrices en memoria
> 📷 *Captura sugerida: la matriz de notas con una celda iluminada.*

Muestra las matrices **tal como están dentro del servidor C++**, con sus valores crudos:

| Matriz | Tamaño reservado | Qué guarda |
|---|---|---|
| `notas` | 100 × 15 | Fila = estudiante, columna = curso. `-1` = sin nota. |
| `asistencia` | 100 × 5 | Fila = estudiante, columna = día. `1` presente, `0` ausente. |
| `adyacencia` | 15 × 15 | `1` si el curso de la fila es prerrequisito del de la columna. |
| `horario` | 5 × 6 | Fila = día, columna = periodo; cada celda tiene sus cursos. |

La pantalla se actualiza sola cada 2 segundos. Para comprobar que un registro se guarda:

1. Abre **Matrices en memoria** en una pestaña y **Calificaciones** en otra.
2. Cambia una nota en Calificaciones.
3. Vuelve a Matrices en memoria: la celda `notas[fila][columna]` que cambió se ilumina en amarillo.

La fila gris marcada como **libre** es la que ocupará el próximo estudiante que registres.

💡 **Consejo:** la ventana del servidor también muestra cada cambio, por ejemplo `[matriz] notas[3][4] = 85 (0901-26-1004, PRG102)`.

---

## 14. Cómo se guardan los datos

- **Se guardan solos:** después de cada cambio que sale bien (registrar, editar, borrar, poner una nota, etc.) el sistema actualiza los archivos de la carpeta `data/`. No hay botón de guardar.
- **Al encender**, el sistema lee esos archivos y continúa donde se quedó.
- Si los archivos faltan o están dañados, arranca con los **datos de ejemplo**.

| Archivo en `data/` | Contiene |
|---|---|
| `estudiantes.json` | Estudiantes |
| `cursos.json` | Cursos |
| `docentes.json` · `asignaciones.json` | Docentes y qué curso da cada uno |
| `notas.json` | Calificaciones |
| `asistencia.json` | Asistencia de la semana |
| `asignaciones_estudiantes.json` | Inscripciones |
| `horario.json` | Horario |
| `avisos.json` | Avisos y calendario |

> 💡 **Copia de seguridad:** antes de una presentación, copia la carpeta `data/` a otro lugar. Para volver a ese punto, cierra el sistema y pega de nuevo la carpeta.

---

## 15. Usar el sistema desde otro dispositivo

Puedes abrir el sistema desde un celular u otra computadora con un **enlace público temporal** (túnel de Cloudflare, gratuito).

1. Compila el sistema si no lo has hecho: `.\compilar`
2. En la terminal escribe:

   ```
   .\tunel
   ```

3. Aparece en verde un enlace como `https://palabras-al-azar.trycloudflare.com`. Ábrelo en cualquier dispositivo.
4. Para cerrarlo, presiona `Ctrl + C`.

> ⚠️ **Importante**
> - Cualquiera con el enlace puede **ver y modificar** los datos: compártelo solo con quien deba usarlo.
> - Funciona solo mientras tu computadora esté encendida y la ventana del túnel abierta.
> - El enlace **cambia** cada vez que abres el túnel.

---

## 16. Solución de problemas

| Problema | Solución |
|---|---|
| El indicador dice **"Servidor desconectado"** | El servidor está apagado. Enciéndelo con `.\compilar ejecutar` y recarga la página (`F5`). |
| La página tarda varios segundos en cargar | Entra por `http://127.0.0.1:8080` en vez de `localhost`. |
| **"No se pudo abrir el puerto 8080"** | El sistema ya está abierto en otra ventana. Ciérralo o usa ese mismo. |
| **"No se pudo reemplazar build\gestion_escolar.exe"** al compilar | El servidor sigue abierto. Ciérralo con `Ctrl + C` y vuelve a compilar. |
| **"No se encontró el compilador de Visual Studio"** | Instala Visual Studio 2022 con "Desarrollo para el escritorio con C++". |
| No me deja inscribir a un estudiante | Revisa el estado del curso: si está **Bloqueado**, la tabla dice qué prerrequisito falta aprobar. |
| Una nota no se guarda | Debe estar entre 0 y 100. Si es inválida, aparece un aviso y la celda vuelve a su valor anterior. |
| Se ve el icono viejo en la pestaña | Recarga sin caché con `Ctrl + F5`. |
| El túnel dice que no encuentra `cloudflared` | Instálalo con `winget install --id Cloudflare.cloudflared`. |

---

## 17. Glosario

| Término | Significado |
|---|---|
| **Carnet** | Código único del estudiante (`0901-26-1009`). |
| **Créditos** | Peso de un curso; se usa para el promedio ponderado. |
| **Prerrequisito** | Curso que hay que aprobar antes de inscribir otro. |
| **Choque de horario** | Un docente o estudiante con dos cursos a la misma hora. |
| **Matriz** | Tabla de números organizada en filas y columnas. |
| **Árbol** | Estructura jerárquica: un nodo raíz con hijos, que a su vez tienen hijos. |
| **Grafo** | Conjunto de nodos unidos por flechas (aristas). |
| **BFS / DFS** | Formas de recorrer un grafo: por niveles (anchura) o hasta el fondo de cada camino (profundidad). |
| **Túnel** | Enlace público temporal para abrir el sistema desde internet. |

---

*CLARD Academy · Proyecto educativo UMG · Ingeniería en Sistemas*
