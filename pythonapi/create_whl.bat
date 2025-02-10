@echo off
set PYTHONROOT=%1
set EXTENSIONFOLDER=%2
set WHEELFOLDER=..\..\output\release\%3
rd /q/s %WHEELFOLDER%
mkdir %WHEELFOLDER%
copy setup.py %WHEELFOLDER%
copy ..\core\version.cpp %WHEELFOLDER%
cd %WHEELFOLDER%
xcopy /s ..\%EXTENSIONFOLDER% .
rem for now remove opencv; needs more testing
rd /q/s ilwis\ilwisobjects\extensions\opencv
%PYTHONROOT%\python.exe ./setup.py bdist_wheel
rd /q/s build
rd /q/s ilwis
rd /q/s ilwis.egg-info
move dist\*.whl .
rd dist
del setup.py
del version.cpp
