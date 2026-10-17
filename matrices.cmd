@echo off
rem Muestra las matrices del servidor en la consola. Ver matrices.ps1 para los detalles.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0matrices.ps1" %*
