@echo off
setlocal
pushd %~dp0

set build_dir=build1

@echo on
cmake --build %build_dir% --config Release --target installer
