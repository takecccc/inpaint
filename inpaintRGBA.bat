@echo on
if "%~1"=="" (
echo "Please specify the image file. "
exit /b
)
set BIN="%~dp0\inpaint.exe"

%BIN% --src "%~dpnx1" --mask "%~dpnx1" --dst "%~dpnx1"
pause