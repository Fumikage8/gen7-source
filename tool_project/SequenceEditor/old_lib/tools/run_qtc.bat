@echo off
rem run_qtc.bat [ディレクトリのパス]
rem 引数で指定したディレクトリのファイルを調べて、mocとuicを実行する
rem Q_OBJECTが記述されたヘッダファイルからは"moc_ヘッダファイル名.cpp"というソースファイルを生成
rem Qｔデザインツールで作ったUIファイルからは"ui_UIファイル名.h"というヘッダファイルを生成
rem プロジェクトのビルド前イベントに"(相対パス)bin/run_qtc.bat $(InputDir)"のように記述して使用してください
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
rem 指定されたディレクトリのファイルを調べる
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
rem タイムスタンプを見て更新されていない場合は処理しない
set moc_file=moc_%~n1.cpp
set h_file=%~1
call :CHECK_TIME %h_file% %moc_file%
if %errorlevel%==1 goto :MOC_OK
exit /b 0
:MOC_OK
rem 中にQ_OBJECTが記述されているか調べる
for /f "eol=/ usebackq" %%i in ("%h_file%") do (
	if "%%i" == "Q_OBJECT" (
		rem mocを実行
		start /w %EXEDIR%moc.exe -o %moc_file% %h_file%
rem		if not %errorlevel%==0 goto :ERROR_MOC
		echo MOC "%h_file%" "%moc_file%" was generated.
		exit /b 0
rem		:ERROR_MOC
rem		@echo MOC %h_file% でエラーが起こりました
rem		pause
rem		exit /b 0
	) else (
		rem echo %%i
	)
)
exit /b 0

:CHECK_UIC
rem すでに自動生成ヘッダ存在するか調べる
for %%i in (%2*) do (
	if "%%~nxi" == "ui_%~n1.h" (
		(exit /b 1)
	)
)
rem 新規ファイルならuicを実行
start /w %EXEDIR%uic.exe -o ui_%~n1.h %1
if not %errorlevel%==0 goto :ERROR_UIC
echo UIC %~nx1 	"ui_%~n1.h" was generated.
exit /b 0
:ERROR_UIC
@echo UI %1 でエラーが起こりました
pause
exit /b 0

:CHECK_RCC

rem タイムスタンプを見て更新されていない場合は処理しない
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
@echo リソース %1 でエラーが起こりました
pause
exit /b 0

:CHECK_TIME
if %FORCE% == 1 goto :UPDATED_MOC
if "%~t1" GTR "%~t2" goto :UPDATED_MOC
exit /b 0
:UPDATED_MOC
exit /b 1

rem 終了
:END
rem pause
@echo on
