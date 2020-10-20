rem @echo off

set FILE_NAME=%~f1
set OUTPUT_FILE_NAME=%2
set ENTRY_POINT=%3
set SHADER_TYPE=%4
set IS_DEBUG=%5
set DEBUG_OPTION="/Zi /Od /Ges /Zpc /O0"

echo �V�F�[�_�[ %FILE_NAME% ���r���h���Ă��܂��B

set COMPILER="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Utilities\bin\x86\fxc.exe"
if "%IS_DEBUG%" EQU "--Debug" (
	%COMPILER% "%FILE_NAME%" "/E" "%ENTRY_POINT%" "/T" "%SHADER_TYPE%" "/Fo" "%OUTPUT_FILE_NAME%" "%DEBUG_OPTION%"
) else (
	%COMPILER% "%FILE_NAME%" "/E" "%ENTRY_POINT%" "/T" "%SHADER_TYPE%" "/Fo" "%OUTPUT_FILE_NAME%"
)
set COMVERTER="Bin2Ascii.exe"
%COMVERTER% "%OUTPUT_FILE_NAME%" "%OUTPUT_FILE_NAME%.txt" "1" "-nosecure" "-nopadding"

if %ERRORLEVEL% NEQ 0 goto :builderr
goto :end

:builderr
echo �r���h�Ɏ��s���܂����B

:end

