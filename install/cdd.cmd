@echo off
set pushd_tmp=%TEMP%\pushd.tmp
set cdd_tmp_cmd=%TEMP%\cdd.tmp.cmd
pushd > %pushd_tmp%
%~dps0_cdd.exe %* < %pushd_tmp% > %cdd_tmp_cmd%
%cdd_tmp_cmd%
