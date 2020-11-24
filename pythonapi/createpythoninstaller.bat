@echo off
copy/y ..\core\buildnumber.h ..\..\output\release
copy/y installerPy.nsi ..\..\output\release
cd ..\..\output\release
echo @echo off > crpi.bat
echo set str=%%3 >> crpi.bat
echo if "%%2" == "ILWIS_BUILDNR" ( >> crpi.bat
echo	if "%%4" == "ILWIS_BUILDNR" ( >> crpi.bat
echo		echo^|set /p="%%str:~0,8%%" ^>^> installerPyVer.nsi >> crpi.bat
echo	) >> crpi.bat
echo ) >> crpi.bat
echo if "%%2" == "ILWIS_VERSION_TYPE" ( >> crpi.bat
echo	if "%%4" == "ILWIS_VERSION_TYPE" ( >> crpi.bat
echo		echo^|set /p="%%str:~1,-2%%" ^>^> installerPyVer.nsi >> crpi.bat
echo	) >> crpi.bat
echo ) >> crpi.bat
echo if "%%2" == "ILWIS_VERSION_NUMBER" ( >> crpi.bat
echo	if "%%4" == "ILWIS_VERSION_NUMBER" ( >> crpi.bat
echo		echo^|set /p="%%str:~0,-1%%" ^>^> installerPyVer.nsi >> crpi.bat
echo	) >> crpi.bat
echo ) >> crpi.bat
echo|set /p="!define ILWIS_VERSION '" > installerPyVer.nsi
for /f "tokens=1,* delims=]" %%j in (buildnumber.h) do call crpi.bat %%j ILWIS_VERSION_TYPE
for /f "tokens=1,* delims=]" %%j in (buildnumber.h) do call crpi.bat %%j ILWIS_VERSION_NUMBER
echo|set /p="-" >> installerPyVer.nsi
for /f "tokens=1,* delims=]" %%j in (buildnumber.h) do call crpi.bat %%j ILWIS_BUILDNR
echo|set /p="'" >> installerPyVer.nsi
echo. >> installerPyVer.nsi
type installerPy.nsi >> installerPyVer.nsi
"C:\Program Files (x86)\NSIS\makensis.exe" /V4 installerPyVer.nsi
del installerPyVer.nsi
del crpi.bat
del installerPy.nsi
del buildnumber.h
