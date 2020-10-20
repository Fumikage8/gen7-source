
@echo off
rem		%1	:	入力gfmot
rem		%2	:	出力dir


CALL %NIJI_ENVIRONMENT_ROOT%/macro_setting.bat


set	__GFMOT__=%1
set	__TARGET_NAME__=%~n1.gfbmot
set	__OUTPATH__=%2\mot\%~n1\%__TARGET_NAME__%
set	__CONVERTER__=%NIJI_TOOL_DIR%\AnimationConverter/AnimationConverter.exe

rem    $(NIJI_TOOL_DIR)/AnimationConverter/AnimationConverter.exe -i $(srcPath) -o $(dstPath)


%__CONVERTER__% -i %__GFMOT__% -o %__OUTPATH__%
@echo	%__OUTPATH__%

