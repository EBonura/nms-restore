@echo off
resetsat /c
psymake
if errorlevel 1 goto done
call db.bat
:done
