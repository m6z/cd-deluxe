@echo off
set path=%~dp0;%path%

chdir/d C:\
pushd "%~dp0"

echo Welcome to the Cd Deluxe shell.  This is primarily for demonstrating the cdd command.
echo.
echo The Cd Deluxe command "cdd" is typically a drop in replacement for the default "cd" command.
echo The "cd" command is aliased to "cdd" via the following "doskey" command:

:: Setup a temporary prompt
set prompt=$q$g 

@echo on
doskey cd="%~dp0%cdd.cmd" $*
@echo off

:: Setup a fancy prompt
set prompt=$q$g $p$_$q$g 

echo.
echo Typing "cd +?" shows the history of all directories visited in this session.
echo Try changing directories and then issue the "cd +?" command again.
echo.

echo cd +?
call %~dps0cdd.cmd +?
