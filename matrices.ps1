# Muestra en consola las matrices del servidor y se actualiza sola (las celdas que cambian salen en verde).
#   Uso:  .\matrices.cmd                  (notas, asistencia y adyacencia)
#         .\matrices.cmd -Matriz notas    (solo una: notas | asistencia | grafo)
#         .\matrices.cmd -Segundos 1 -Puerto 9090
# Requisito: el servidor debe estar corriendo (.\compilar.cmd ejecutar). Ctrl+C para salir.
param(
    [ValidateSet("todas", "notas", "asistencia", "grafo")][string]$Matriz = "todas",
    [int]$Segundos = 2,
    [int]$Puerto = 8080
)

[Console]::OutputEncoding = [Text.Encoding]::UTF8
$base = "http://127.0.0.1:$Puerto"
$anteriores = @{}   # valores del refresco anterior, para resaltar cambios

function Obtener($ruta) {
    $r = Invoke-WebRequest "$base$ruta" -UseBasicParsing -TimeoutSec 3
    return [Text.Encoding]::UTF8.GetString($r.RawContentStream.ToArray()) | ConvertFrom-Json
}

function Ajustar([string]$texto, [int]$ancho) {
    if ($texto.Length -gt $ancho) { $texto = $texto.Substring(0, $ancho) }
    return $texto.PadRight($ancho)
}

# Dibuja una matriz con etiquetas de fila y columna. $formato convierte cada valor en texto.
function Dibujar($titulo, $filas, $columnas, $matriz, [scriptblock]$formato) {
    $anchoFila = 22
    $anchoCelda = 7
    Write-Host ""
    Write-Host "$titulo  ($($filas.Count) x $($columnas.Count))" -ForegroundColor Cyan
    Write-Host ((" " * $anchoFila) + (($columnas | ForEach-Object { (Ajustar $_ ($anchoCelda - 1)).Trim().PadLeft($anchoCelda) }) -join "")) -ForegroundColor DarkGray
    for ($i = 0; $i -lt $filas.Count; $i++) {
        Write-Host (Ajustar "$i $($filas[$i])" $anchoFila) -NoNewline -ForegroundColor DarkGray
        for ($j = 0; $j -lt $columnas.Count; $j++) {
            $valor = $null
            if ($i -lt $matriz.Count -and $j -lt @($matriz[$i]).Count) { $valor = @($matriz[$i])[$j] }
            $texto = (& $formato $valor).PadLeft($anchoCelda)
            $clave = "$titulo|$i|$j"
            $color = if ($anteriores.ContainsKey($clave) -and $anteriores[$clave] -ne $texto) { "Green" } elseif ($null -eq $valor) { "DarkGray" } else { "Gray" }
            $anteriores[$clave] = $texto
            Write-Host $texto -NoNewline -ForegroundColor $color
        }
        Write-Host ""
    }
}

while ($true) {
    try {
        $notas = $null; $asistencia = $null; $grafo = $null
        if ($Matriz -in "todas", "notas")      { $notas = Obtener "/api/notas" }
        if ($Matriz -in "todas", "asistencia") { $asistencia = Obtener "/api/asistencia" }
        if ($Matriz -in "todas", "grafo")      { $grafo = Obtener "/api/grafo" }

        Clear-Host
        Write-Host "Matrices en memoria del servidor $base   (cada $Segundos s, Ctrl+C para salir)   $(Get-Date -Format HH:mm:ss)" -ForegroundColor Yellow

        if ($notas) {
            Dibujar "NOTAS [estudiante][curso]" @($notas.estudiantes | ForEach-Object { $_.nombre }) @($notas.cursos | ForEach-Object { $_.codigo }) $notas.matriz {
                param($v) if ($null -eq $v) { "--" } else { "{0:0.#}" -f [double]$v }
            }
        }
        if ($asistencia) {
            Dibujar "ASISTENCIA [estudiante][dia]  (1 presente, 0 ausente, - sin registro)" @($asistencia.estudiantes | ForEach-Object { $_.nombre }) @($asistencia.dias) $asistencia.matriz {
                param($v) if ($null -eq $v -or [int]$v -lt 0) { "-" } else { "$v" }
            }
        }
        if ($grafo) {
            $codigos = @($grafo.nodos | ForEach-Object { $_.codigo })
            Dibujar "ADYACENCIA [curso][curso]  (1 = la fila es prerrequisito de la columna)" $codigos $codigos $grafo.matriz {
                param($v) if ([int]$v -eq 1) { "1" } else { "." }
            }
        }
    } catch {
        Clear-Host
        Write-Host "No se pudo leer $base ($($_.Exception.Message))." -ForegroundColor Red
        Write-Host "Arranca el servidor con: .\compilar.cmd ejecutar   (reintentando...)"
    }
    Start-Sleep -Seconds $Segundos
}
