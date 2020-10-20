@echo off

@REM ############################################################################
@REM  環境設定
CALL %NIJI_ENVIRONMENT_ROOT%/macro_setting.bat


@REM ############################################################################
@REM  ローカル変数
SETLOCAL
SET LOCAL_ERRORLEVEL=0


@REM ############################################################################
@REM  実行

@REM ####################################
@REM  ctr

del %NIJI_CONV_HEADER_NIJI_DIR%\mobiclip_resource\ctr\mobiclip_resource_pack.h
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

del %NIJI_ARC_INDEX_DIR%\mobiclip_resource.gaix
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_CTR_DIR%\mobiclip_resource.garc
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_CTR_DIR%\mobiclip_resource.gasc
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1


@REM ####################################
@REM  win32

del %NIJI_CONV_HEADER_NIJI_DIR%\mobiclip_resource\win32\mobiclip_resource_pack.h
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

del %NIJI_ARC_INDEX_DIR%\mobiclip_resource.gaix
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_WIN32_DIR%\mobiclip_resource.garc
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_WIN32_DIR%\mobiclip_resource.gasc
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1


@REM ############################################################################
@REM  終了
@if not %LOCAL_ERRORLEVEL% == 0 (
  echo ERROR: エラーが発生しました。
) else (
  echo SUCCESS: 正常に終了しました。
)

pause

@REM  exitの前に暗黙のENDLOCALがあります
@if not %LOCAL_ERRORLEVEL% == 0 (
  exit /b 1
) else (
  exit /b 0
)
