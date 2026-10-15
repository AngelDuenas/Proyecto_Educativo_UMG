/* =====================================================================
   Vista Álgebra lineal aplicada (de muestra, solo lectura)
   ---------------------------------------------------------------------
   GET /api/algebra →
     { estudiantes: [{carnet, nombre}], cursos: [{codigo, nombre, creditos}],
       A: n×m (null = sin nota), transpuesta: m×n,
       creditos: c, productoAc: A·c, creditosConNota, promedioPonderado, promedioSimple,
       promedioCursos: (1/n)·Aᵀ·1,
       combinacion: { pesoNotas, pesoAsistencia, asistencia, resultado },
       adyacencia: { codigos, M, M2, cadenas: [{desde, pasaPor, hasta}] } }
   ===================================================================== */

const contenedor = document.getElementById("contenido-algebra");

const num = (x, d = 1) => (x === null || x === undefined ? "—" : formatear(x, d));

// Tabla de una matriz con encabezados de fila y columna.
function tablaMatriz(filas, columnas, valores, { decimales = 0, clase = () => "" } = {}) {
  const cab = columnas.map((c) => `<th class="text-center">${esc(c)}</th>`).join("");
  const cuerpo = valores
    .map(
      (fila, i) =>
        `<tr><th>${esc(filas[i])}</th>${fila.map((v, j) => `<td class="celda ${clase(v, i, j)}">${num(v, decimales)}</td>`).join("")}</tr>`
    )
    .join("");
  return `<div class="tabla-contenedor"><table class="tabla matriz"><thead><tr><th></th>${cab}</tr></thead><tbody>${cuerpo}</tbody></table></div>`;
}

function seccion(id, numero, titulo, formula, explicacion, cuerpo) {
  return `<section class="panel mb-5" id="${id}" aria-labelledby="${id}-t">
    <div class="panel-encabezado">
      <div>
        <h2 id="${id}-t">${numero}. ${titulo}</h2>
        <p>${explicacion}</p>
      </div>
    </div>
    <div class="formula">${formula}</div>
    ${cuerpo}
  </section>`;
}

function dibujar(d) {
  const n = d.estudiantes.length;
  const m = d.cursos.length;
  const nombres = d.estudiantes.map((e) => e.nombre.split(" ").slice(0, 2).join(" "));
  const codigos = d.cursos.map((c) => c.codigo);

  // 1. Matriz A
  const s1 = seccion(
    "sec-matriz", 1, "La matriz de notas A",
    `A ∈ ℝ<sup>${n}×${m}</sup> &nbsp;·&nbsp; a<sub>ij</sub> = nota del estudiante <i>i</i> en el curso <i>j</i>`,
    "Cada fila es un estudiante y cada columna un curso. Es la misma matriz que se edita en Calificaciones; las celdas vacías cuentan como 0 en los productos.",
    `<span class="dimension">${n} × ${m}</span>` + tablaMatriz(nombres, codigos, d.A, { clase: (v) => claseNota(v) })
  );

  // 2. Transpuesta
  const s2 = seccion(
    "sec-transpuesta", 2, "Transpuesta Aᵀ",
    `(Aᵀ)<sub>ji</sub> = a<sub>ij</sub> &nbsp;·&nbsp; Aᵀ ∈ ℝ<sup>${m}×${n}</sup>`,
    "Las filas pasan a ser columnas: ahora cada fila es un curso. Sirve para trabajar «por curso» con las mismas operaciones que usamos «por estudiante».",
    `<span class="dimension">${m} × ${n}</span>` + tablaMatriz(codigos, nombres, d.transpuesta, { clase: (v) => claseNota(v) })
  );

  // 3. Producto matriz por vector con los créditos
  const vectorC = d.cursos.map((c, j) => `<span><small>${esc(c.codigo)}</small><b>${num(d.creditos[j], 0)}</b></span>`).join("");
  const filasProducto = d.estudiantes
    .map((e, i) => {
      const dif = d.promedioPonderado[i] - d.promedioSimple[i];
      return `<tr>
        <td>${esc(e.nombre)}<span class="sub">${esc(e.carnet)}</span></td>
        <td class="numero">${num(d.productoAc[i], 0)}</td>
        <td class="numero">${num(d.creditosConNota[i], 0)}</td>
        <td class="numero"><b>${num(d.promedioPonderado[i], 2)}</b></td>
        <td class="numero">${num(d.promedioSimple[i], 2)}</td>
        <td class="numero ${dif >= 0 ? "text-azul-700" : "text-naranja-700"}">${dif >= 0 ? "+" : ""}${num(dif, 2)}</td>
      </tr>`;
    })
    .join("");
  const s3 = seccion(
    "sec-producto", 3, "Producto matriz × vector: promedio ponderado",
    `A · c = <i>s</i> &nbsp;·&nbsp; s<sub>i</sub> = Σ<sub>j</sub> a<sub>ij</sub> · c<sub>j</sub> &nbsp;·&nbsp; promedio<sub>i</sub> = s<sub>i</sub> / Σ c<sub>j</sub>`,
    "c es el vector de créditos de cada curso. Multiplicar A por c da, para cada estudiante, la suma de sus notas pesadas por créditos; al dividir entre los créditos se obtiene el promedio ponderado. Un curso de 5 créditos pesa más que uno de 4.",
    `<p class="mb-2 text-sm font-bold text-marino">Vector c (créditos)</p><div class="vector mb-4">${vectorC}</div>
     <div class="tabla-contenedor"><table class="tabla">
       <thead><tr><th>Estudiante</th><th class="numero">(A·c)<sub>i</sub></th><th class="numero">Σ créditos</th><th class="numero">Ponderado</th><th class="numero">Simple</th><th class="numero">Diferencia</th></tr></thead>
       <tbody>${filasProducto}</tbody>
     </table></div>`
  );

  // 4. Promedio por curso
  const barras = d.cursos
    .map(
      (c, j) => `<li class="flex items-center gap-3">
        <span class="w-16 flex-none font-bold">${esc(c.codigo)}</span>
        <div class="barra-pista"><div class="barra-relleno ${d.promedioCursos[j] < CONFIG.NOTA_APROBACION ? "mal" : d.promedioCursos[j] < CONFIG.NOTA_RIESGO ? "alerta" : ""}" style="width:${Math.max(0, Math.min(100, d.promedioCursos[j]))}%"></div></div>
        <span class="barra-valor">${num(d.promedioCursos[j], 1)}</span>
      </li>`
    )
    .join("");
  const s4 = seccion(
    "sec-cursos", 4, "Promedio por curso con Aᵀ",
    `p = (1/n) · Aᵀ · <b>1</b> &nbsp;·&nbsp; <b>1</b> = (1, 1, …, 1) ∈ ℝ<sup>${n}</sup>`,
    "Multiplicar la transpuesta por un vector de unos suma cada columna de A (todas las notas de un curso). Al dividir entre el número de estudiantes queda el promedio del curso.",
    `<ul class="grid gap-2">${barras}</ul>`
  );

  // 5. Combinación lineal
  const k = d.combinacion;
  const filasComb = d.estudiantes
    .map(
      (e, i) => `<tr><td>${esc(e.nombre)}</td>
        <td class="numero">${num(d.promedioPonderado[i], 2)}</td>
        <td class="numero">${num(k.asistencia[i], 1)} %</td>
        <td class="numero"><b>${num(k.resultado[i], 2)}</b></td></tr>`
    )
    .join("");
  const s5 = seccion(
    "sec-combinacion", 5, "Combinación lineal de vectores",
    `r = ${num(k.pesoNotas, 1)} · p + ${num(k.pesoAsistencia, 1)} · a`,
    `p es el vector de promedios ponderados y a el de porcentajes de asistencia. Una combinación lineal suma vectores multiplicados por escalares: aquí la nota pesa ${num(k.pesoNotas * 100, 0)} % y la asistencia ${num(k.pesoAsistencia * 100, 0)} %, como un indicador de desempeño.`,
    `<div class="tabla-contenedor"><table class="tabla">
       <thead><tr><th>Estudiante</th><th class="numero">p (promedio)</th><th class="numero">a (asistencia)</th><th class="numero">r = ${num(k.pesoNotas, 1)}p + ${num(k.pesoAsistencia, 1)}a</th></tr></thead>
       <tbody>${filasComb}</tbody>
     </table></div>`
  );

  // 6. Matriz de adyacencia al cuadrado
  const ady = d.adyacencia;
  const cadenas = ady.cadenas.length
    ? `<ul class="recorrido-lista mt-4">${ady.cadenas
        .map((c) => `<li><b>2</b>${esc(c.desde)} → ${c.pasaPor.map(esc).join(" / ")} → ${esc(c.hasta)}</li>`)
        .join("")}</ul>`
    : htmlVacio("No hay cadenas de dos pasos.", "");
  const s6 = seccion(
    "sec-adyacencia", 6, "Potencia de una matriz: M²",
    `(M²)<sub>ij</sub> = Σ<sub>k</sub> m<sub>ik</sub> · m<sub>kj</sub>`,
    "M es la matriz de adyacencia del grafo de prerrequisitos (m<sub>ij</sub> = 1 si el curso i es prerrequisito de j). Al multiplicarla por sí misma, cada celda de M² cuenta los caminos de dos pasos: cursos que se necesitan «de forma indirecta».",
    `<div class="grid gap-4 min-[1700px]:grid-cols-2">
       <div><p class="mb-2 text-sm font-bold text-marino">M (prerrequisito directo)</p>${tablaMatriz(ady.codigos, ady.codigos, ady.M, { clase: (v) => (v > 0 ? "uno" : "cero") })}</div>
       <div><p class="mb-2 text-sm font-bold text-marino">M² = M · M (a dos pasos)</p>${tablaMatriz(ady.codigos, ady.codigos, ady.M2, { clase: (v) => (v > 0 ? "uno" : "cero") })}</div>
     </div>
     <p class="mt-4 text-sm font-bold text-marino">Cadenas encontradas (celdas de M² mayores que 0)</p>${cadenas}`
  );

  contenedor.innerHTML = s1 + s2 + s3 + s4 + s5 + s6;
  contenedor.querySelectorAll("#sec-adyacencia table").forEach((t) => t.classList.add("adyacencia"));
}

async function cargar() {
  try {
    dibujar(await api("/algebra"));
  } catch (e) {
    contenedor.innerHTML = htmlError(`No se pudieron calcular las operaciones: ${e.message}`);
  }
}

cargar();
