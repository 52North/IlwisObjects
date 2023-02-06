@echo off
rd /q/s ..\..\output\release\python_whl
mkdir ..\..\output\release\python_whl
copy setup.py ..\..\output\release\python_whl
cd ..\..\output\release\python_whl
xcopy/s ..\ilwispy\ .
rem for now remove opencv; needs more testing
rd /q/s ilwis\ilwisobjects\extensions\opencv
C:\Python36\python.exe ./setup.py bdist_wheel -universal
rd /q/s build
rd /q/s ilwis
rd /q/s ilwispy.egg-info
move dist\*.whl .
rd dist
del setup.py
cd ..\..\..\projects\pythonapi