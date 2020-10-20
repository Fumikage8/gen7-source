@echo off


@REM ############################################################################
@REM  多重CALL防止
if defined GFL2_RENDERINGENGINE_RESOURCE_MOBICLIP_RESOURCE_MACRO_SETTING_CALL (
  @REM CALL済み
  exit /b 0
) else (
  @REM 初回CALL
  SET GFL2_RENDERINGENGINE_RESOURCE_MOBICLIP_RESOURCE_MACRO_SETTING_CALL=true
)


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

%NIJI_RUBY_1_8% %BINLINKER_SCR% ^
    ../simple_board_model_out/binary/ctr/simple_board_model.ctr.gfbmdl ^
    ../simple_board_model_out/binary/ctr/FillScrean.ctr.gfbvsh ^
    ../simple_board_model_out/binary/ctr/Comb1_Manual.ctr.gfbfsh ^
    ../simple_board_model_out/binary/ctr/simple_dummy_tex.ctr.btex ^
    ctr/mobiclip_resource_pack.pack BL 128
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

%GARC_EXE% --sort=name_up ctr/mobiclip_resource ctr/mobiclip_resource_pack.pack
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

@copy ctr\mobiclip_resource_pack.h %NIJI_CONV_HEADER_NIJI_DIR%\mobiclip_resource\ctr
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy ctr\mobiclip_resource.gaix   %NIJI_ARC_INDEX_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy ctr\mobiclip_resource.garc   %NIJI_ARC_CTR_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy ctr\mobiclip_resource.gasc   %NIJI_ARC_CTR_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1


@REM ####################################
@REM  win32

%NIJI_RUBY_1_8% %BINLINKER_SCR% ^
    ../simple_board_model_out/binary/wingl/simple_board_model.wingl.gfbmdl ^
    ../simple_board_model_out/binary/wingl/FillScrean.wingl.gfbvsh ^
    ../simple_board_model_out/binary/wingl/Comb1_Manual.wingl.gfbfsh ^
    ../simple_board_model_out/binary/wingl/simple_dummy_tex.wingl.btex ^
    win32/mobiclip_resource_pack.pack BL 128
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

%GARC_EXE% --sort=name_up win32/mobiclip_resource win32/mobiclip_resource_pack.pack
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

@copy win32\mobiclip_resource_pack.h %NIJI_CONV_HEADER_NIJI_DIR%\mobiclip_resource\win32
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy win32\mobiclip_resource.gaix   %NIJI_ARC_INDEX_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy win32\mobiclip_resource.garc   %NIJI_ARC_WIN32_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy win32\mobiclip_resource.gasc   %NIJI_ARC_WIN32_DIR%
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
