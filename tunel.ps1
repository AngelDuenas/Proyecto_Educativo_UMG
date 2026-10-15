# Publica el sistema en internet con un túnel gratuito de Cloudflare (Quick Tunnel).
#   Uso:  .\tunel.cmd            (o: powershell -ExecutionPolicy Bypass -File .\tunel.ps1)
#         .\tunel.cmd -Puerto 9090
# Requisitos: haber compilado el backend (build\gestion_escolar.exe) y tener cloudflared:
#         winget install --id Cloudflare.cloudflared
# El enlace (https://algo.trycloudflare.com) cambia cada vez y deja de funcionar al cerrar esta ventana.
# ATENCIÓN: la API no tiene contraseña; cualquiera con el enlace puede ver y modificar los datos.
param([int]$Puerto = 8080)

$ErrorActionPreference = "Stop"
$raiz = $PSScriptRoot
$servidorExe = Join-Path $raiz "build\gestion_escolar.exe"
$salud = "http://127.0.0.1:$Puerto/api/salud"

function Responde {
    try { Invoke-RestMethod $salud -TimeoutSec 2 | Out-Null; return $true } catch { return $false }
}

# ---- cloudflared ----
$cloudflared = Get-Command cloudflared -ErrorAction SilentlyContinue
if ($cloudflared) { $cloudflared = $cloudflared.Source }
elseif (Test-Path "${env:ProgramFiles(x86)}\cloudflared\cloudflared.exe") { $cloudflared = "${env:ProgramFiles(x86)}\cloudflared\cloudflared.exe" }
elseif (Test-Path "$env:ProgramFiles\cloudflared\cloudflared.exe") { $cloudflared = "$env:ProgramFiles\cloudflared\cloudflared.exe" }
else {
    Write-Host "No se encontró cloudflared. Instálalo con:" -ForegroundColor Red
    Write-Host "  winget install --id Cloudflare.cloudflared"
    exit 1
}

# ---- servidor C++ ----
$servidor = $null
if (Responde) {
    Write-Host "El servidor C++ ya está corriendo en el puerto $Puerto." -ForegroundColor Cyan
} else {
    if (-not (Test-Path $servidorExe)) {
        Write-Host "No existe $servidorExe. Primero compila el backend (ver README)." -ForegroundColor Red
        exit 1
    }
    Write-Host "Iniciando el servidor C++ en el puerto $Puerto..." -ForegroundColor Cyan
    $env:PUERTO = "$Puerto"
    # Se ejecuta desde la raíz del repo para que encuentre frontend\ y data\
    $servidor = Start-Process -FilePath $servidorExe -WorkingDirectory $raiz -WindowStyle Hidden -PassThru
    $listo = $false
    for ($i = 0; $i -lt 20 -and -not $listo; $i++) { Start-Sleep -Milliseconds 500; $listo = Responde }
    if (-not $listo) {
        Write-Host "El servidor no respondió en $salud." -ForegroundColor Red
        if (-not $servidor.HasExited) { Stop-Process -Id $servidor.Id -Force }
        exit 1
    }
}

# ---- túnel ----
Write-Host ""
Write-Host "Abriendo el túnel (Ctrl+C para cerrarlo)..." -ForegroundColor Cyan
Write-Host "Recuerda: cualquiera con el enlace puede modificar los datos. Compártelo solo con quien deba verlo." -ForegroundColor Yellow
try {
    # cloudflared escribe el enlace en su registro; se resalta cuando aparece.
    & $cloudflared tunnel --no-autoupdate --url "http://127.0.0.1:$Puerto" 2>&1 | ForEach-Object {
        $linea = "$_"
        if ($linea -match "https://[a-z0-9-]+\.trycloudflare\.com") {
            Write-Host ""
            Write-Host "  Enlace público:  $($Matches[0])" -ForegroundColor Green
            Write-Host ""
        } elseif ($linea -match "ERR|error") {
            Write-Host $linea -ForegroundColor DarkYellow
        }
    }
} finally {
    if ($servidor -and -not $servidor.HasExited) {
        Stop-Process -Id $servidor.Id -Force
        Write-Host "Servidor C++ detenido." -ForegroundColor Cyan
    }
}
