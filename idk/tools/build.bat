set SolnName=%1
set ProjName=%2
set Config=%3
set VsWhere=%4
@echo off
setlocal enabledelayedexpansion

for /f "usebackq tokens=*" %%i in (`%4 -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  "%%i" %SolnName% -target:%ProjName% /p:Configuration=%Config%
  exit /b !errorlevel!
)
cd %dir%
MsBuild.exe 
