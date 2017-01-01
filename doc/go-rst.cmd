@echo off
set filename=cdd.html
del %filename%
D:\App\Python26\python.exe D:\App\Python26\Scripts\rst2html.py --stylesheet cdd.css cdd.rst %filename%
start %filename%

