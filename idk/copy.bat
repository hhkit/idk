set src=%1
set dest=%2
set ext=%3


echo Source: %src%

echo Dest: %dest%

echo Extension: %ext%

setlocal
for /f "tokens=4-5 delims=. " %%i in ('ver') do set VERSION=%%i.%%j

if "%version%" == "10.0" robocopy %src% %dest% *%ext% /e /dcopy:DAT
if "%version%" == "6.1"  robocopy %src% %dest% *%ext% /e /dcopy:T
pause
if (%errorlevel% leq 1) (
exit /b 0
)
else
(
echo Robocopy Exit Code: %errorlevel%
exit /b %errorlevel%
)