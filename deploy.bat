@echo off
@cd /d %~dp0
@mkdir Deploy
@mkdir Deploy\LipaTemplate
@mkdir Deploy\img

@copy LipaTemplate\*.* Deploy\LipaTemplate >nul 2>&1
@copy img\*.* Deploy\img >nul 2>&1

@copy LipaTemplate.sln Deploy\LipaTemplate.sln >nul 2>&1
@copy *.lib Deploy >nul 2>&1
@copy *.dll Deploy >nul 2>&1
@echo %date% %time% >> Deploy\build.dat
prepare.vbs %1
@rmdir Deploy /S /Q