@echo on
if "%~1"=="" (
echo "Please specify the image file. "
exit /b
)
set BIN="%~dp0\inpaint.exe"

for %%F in (%*) do (
%BIN% --src "%%~dpnxF" --mask "%%~dpnxF" --dst "%%~dpnxF"
)
pause