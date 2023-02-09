@echo off
set PYTHONROOT=%1
set EXTENSIONFOLDER=%2
set WHEELFOLDER=%3
rd /q/s ..\..\output\release\%WHEELFOLDER%
mkdir ..\..\output\release\%WHEELFOLDER%
copy setup.py ..\..\output\release\%WHEELFOLDER%
cd ..\..\output\release\%WHEELFOLDER%
xcopy/s ..\%EXTENSIONFOLDER%\ .
rem for now remove opencv; needs more testing
rd /q/s ilwis\ilwisobjects\extensions\opencv
%PYTHONROOT%\python.exe ./setup.py bdist_wheel
rd /q/s build
rd /q/s ilwis
rd /q/s ilwis.egg-info
move dist\*.whl .
rd dist
del setup.py
cd ..\..\..\projects\pythonapi