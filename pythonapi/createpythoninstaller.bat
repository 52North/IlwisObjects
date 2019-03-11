@echo off
copy/y installerPy.nsi ..\..\output\win32release
cd ..\..\output\win32release
"C:\Program Files (x86)\NSIS\makensis.exe" /V4 installerPy.nsi
del installerPy.nsi