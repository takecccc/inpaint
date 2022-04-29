@echo off
set BIN="%~dp0\inpaint.exe"
if "%1"=="" (
echo "Please specify the image file. "
) else (
%BIN% --src "%~dpnx1" --mask "%~dpnx1" --dst "%~dpn1_inpainted%~x1"
)
pause