@echo off
set cdd_exe=C:\Users\Mike\Development\cdd_v2\cd-deluxe\build1\main\Debug\cd-deluxe.exe
set pushd_tmp=%TEMP%\pushd.tmp
set cdd_tmp_cmd=%TEMP%\cdd.tmp.cmd
pushd > %pushd_tmp%
"%cdd_exe%" %* < %pushd_tmp% > %cdd_tmp_cmd%
%cdd_tmp_cmd%
