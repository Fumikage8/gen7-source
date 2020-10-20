@echo off
rem run_qtc.bat [�f�B���N�g���̃p�X]
rem �����Ŏw�肵���f�B���N�g���̃t�@�C���𒲂ׂāAmoc��uic�����s����
rem Q_OBJECT���L�q���ꂽ�w�b�_�t�@�C�������"moc_�w�b�_�t�@�C����.cpp"�Ƃ����\�[�X�t�@�C���𐶐�
rem Q���f�U�C���c�[���ō����UI�t�@�C�������"ui_UI�t�@�C����.h"�Ƃ����w�b�_�t�@�C���𐶐�
rem �v���W�F�N�g�̃r���h�O�C�x���g��"(���΃p�X)bin/run_qtc.bat $(InputDir)"�̂悤�ɋL�q���Ďg�p���Ă�������
echo execute %~nx0

if "%2" == "" (
set FORCE=0
) else (
set FORCE=%2
echo moc all files
)

if "%3" == "" (
set EXEDIR=%QT_DIR_4%/bin/
) else (
set EXEDIR=%3/bin/
)

echo %EXEDIR%

cd %1

call :QT_EXE

for /D /R %%d in (*) do (
	pushd %%d
    call :QT_EXE
    popd
)

goto :END

:QT_EXE
rem �w�肳�ꂽ�f�B���N�g���̃t�@�C���𒲂ׂ�
for %%i in (*.h) do (
	call :CHECK_MOC "%%i"
) 
for %%i in (*.ui) do (
	call :CHECK_UIC "%%i"
) 
for %%i in (*.qrc) do (
	call :CHECK_RCC "%%i"
) 
exit /b 0

:CHECK_MOC
rem �^�C���X�^���v�����čX�V����Ă��Ȃ��ꍇ�͏������Ȃ�
set moc_file=moc_%~n1.cpp
set h_file=%~1
call :CHECK_TIME %h_file% %moc_file%
if %errorlevel%==1 goto :MOC_OK
exit /b 0
:MOC_OK
rem ����Q_OBJECT���L�q����Ă��邩���ׂ�
for /f "eol=/ usebackq" %%i in ("%h_file%") do (
	if "%%i" == "Q_OBJECT" (
		rem moc�����s
		start /w %EXEDIR%moc.exe -o %moc_file% %h_file%
rem		if not %errorlevel%==0 goto :ERROR_MOC
		echo MOC "%h_file%" "%moc_file%" was generated.
		exit /b 0
rem		:ERROR_MOC
rem		@echo MOC %h_file% �ŃG���[���N����܂���
rem		pause
rem		exit /b 0
	) else (
		rem echo %%i
	)
)
exit /b 0

:CHECK_UIC
rem ���łɎ��������w�b�_���݂��邩���ׂ�
for %%i in (%2*) do (
	if "%%~nxi" == "ui_%~n1.h" (
		(exit /b 1)
	)
)
rem �V�K�t�@�C���Ȃ�uic�����s
start /w %EXEDIR%uic.exe -o ui_%~n1.h %1
if not %errorlevel%==0 goto :ERROR_UIC
echo UIC %~nx1 	"ui_%~n1.h" was generated.
exit /b 0
:ERROR_UIC
@echo UI %1 �ŃG���[���N����܂���
pause
exit /b 0

:CHECK_RCC

rem �^�C���X�^���v�����čX�V����Ă��Ȃ��ꍇ�͏������Ȃ�
set rcc_file=qrc_%~n1.cpp
set r_file=%~1
set r_file_full=%1
call :CHECK_TIME %r_file% %rcc_file%
if %errorlevel%==1 goto :RCC_OK
exit /b 0
:RCC_OK

start /w %EXEDIR%rcc.exe %r_file_full% -o %rcc_file%
if not %errorlevel%==0 goto :ERROR_RCC
echo RCC %r_file% %rcc_file% was generated.
exit /b 0
:ERROR_RCC
@echo ���\�[�X %1 �ŃG���[���N����܂���
pause
exit /b 0

:CHECK_TIME
if %FORCE% == 1 goto :UPDATED_MOC
if "%~t1" GTR "%~t2" goto :UPDATED_MOC
exit /b 0
:UPDATED_MOC
exit /b 1

rem �I��
:END
rem pause
@echo on
