
@echo off
rem		%1	:	入力gfmdl
rem		%2	:	出力dir


CALL %NIJI_ENVIRONMENT_ROOT%/macro_setting.bat


set	__GFMDL__=%1
set	__OUTDIR__=%2\mdl\%~n1
set	__TARGET_NAME__=%~n1.ctr.gfbmdlp
set	__PACKER__=%NIJI_TOOL_DIR%\BinaryModelPacker\BinaryModelPacker.exe


rem BinaryModelPacker.exe -m ctr -i pm0025_00\pm0025_00.gfmdl -o pm0025_00\binary_ctr


%__PACKER__% -m ctr -i %__GFMDL__% -o %__OUTDIR__%
@echo	%__OUTDIR__%\%__TARGET_NAME__%

