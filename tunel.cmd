@echo off
rem Abre el túnel público de Cloudflare. Ver tunel.ps1 para los detalles.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tunel.ps1" %*
