call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
set QT_SDK_DIR=C:\Qt\4.5.3
PATH=%PATH%;%QT_SDK_DIR%\bin;
set QMAKESPEC=win32-msvc2008
cd Qt4
rem qmake qscintilla.pro
nmake -f Makefile.debug
nmake -f Makefile.release
pause
exit /b 0
