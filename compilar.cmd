@echo off
rem Compila el backend con Visual Studio (MSVC) sin abrir la "Developer PowerShell".
rem   compilar          -> compila build\gestion_escolar.exe
rem   compilar ejecutar -> compila y arranca el servidor (http://127.0.0.1:8080)
rem Se puede ejecutar desde cualquier carpeta: siempre trabaja en la raiz del proyecto.
setlocal
cd /d "%~dp0"

rem ---- Activar el compilador de Visual Studio si no esta disponible ----
where cl >nul 2>nul && goto compilar
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto sin_vs
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSDIR=%%i"
if not defined VSDIR goto sin_vs
call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>nul
where cl >nul 2>nul || goto sin_vs

:compilar
if not exist build\obj mkdir build\obj
rem Se borra el ejecutable anterior para no confundirlo con uno nuevo si la compilacion falla.
if exist build\gestion_escolar.exe del build\gestion_escolar.exe 2>nul
if exist build\gestion_escolar.exe goto en_uso

echo Compilando...
cl /nologo /std:c++17 /EHsc /O2 /W4 /utf-8 /bigobj /D_WIN32_WINNT=0x0A00 /D_CRT_SECURE_NO_WARNINGS ^
   /external:I backend\third_party /external:W0 /Ibackend\src ^
   /Fobuild\obj\ /Febuild\gestion_escolar.exe backend\src\*.cpp ws2_32.lib wsock32.lib | findstr /v /r /c:"^[a-z_]*\.cpp$"
if not exist build\gestion_escolar.exe goto fallo
echo Listo: build\gestion_escolar.exe

if /i not "%~1"=="ejecutar" goto fin
echo Iniciando el servidor. Ctrl+C para detenerlo.
build\gestion_escolar.exe
goto fin

:sin_vs
echo No se encontro el compilador de Visual Studio.
echo Instala Visual Studio con la carga "Desarrollo para el escritorio con C++".
exit /b 1

:en_uso
echo No se pudo reemplazar build\gestion_escolar.exe: el servidor sigue abierto.
echo Cierralo (Ctrl+C en su ventana) y vuelve a compilar.
exit /b 1

:fallo
echo La compilacion fallo. Revisa los errores de arriba.
exit /b 1

:fin
endlocal
