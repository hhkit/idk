set src=%1
set dest=%2
set ext=%3

echo Source: %src%

echo Dest: %dest%

echo Extension: %ext%

robocopy %src% %dest% *%ext% /e /dcopy:DAT
pause
if (%errorlevel% leq 1) (
exit /b 0
)
else
(
echo Robocopy Exit Code: %errorlevel%
exit /b %errorlevel%
)