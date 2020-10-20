@echo off


@REM ############################################################################
@REM  多重CALL防止
if defined GFL2_RENDERINGENGINE_RESOURCE_BLOOM_RESOURCE_MACRO_SETTING_CALL (
  @REM CALL済み
  exit /b 0
) else (
  @REM 初回CALL
  SET GFL2_RENDERINGENGINE_RESOURCE_BLOOM_RESOURCE_MACRO_SETTING_CALL=true
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
    ../bloom_high_luminance_model_rgbxa_out/binary/ctr/bloom_high_luminance_model_rgbxa.ctr.gfbmdl ^
    ../bloom_high_luminance_model_rgbxa_out/binary/ctr/FillScrean.ctr.gfbvsh ^
    ../bloom_high_luminance_model_rgbxa_out/binary/ctr/Comb1_Manual.ctr.gfbfsh ^
    ../bloom_high_luminance_model_rgbxa_out/binary/ctr/bloom_dummy_tex.ctr.btex ^
    ../bloom_board_model_out/binary/ctr/bloom_board_model.ctr.gfbmdl ^
    ../bloom_board_model_out/binary/ctr/FillScrean.ctr.gfbvsh ^
    ../bloom_board_model_out/binary/ctr/Comb1_Manual.ctr.gfbfsh ^
    ../bloom_board_model_out/binary/ctr/bloom_dummy_tex.ctr.btex ^
    ../tex/binary/ctr/bloom_small_tex_rgba0.ctr.btex ^
    ../tex/binary/ctr/bloom_small_tex_rgba1.ctr.btex ^
    ctr/bloom_resource_pack.pack BL 128
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

%GARC_EXE% --sort=name_up ctr/bloom_resource ctr/bloom_resource_pack.pack
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

@copy ctr\bloom_resource_pack.h %NIJI_CONV_HEADER_NIJI_DIR%\bloom_resource\ctr
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy ctr\bloom_resource.gaix   %NIJI_ARC_INDEX_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy ctr\bloom_resource.garc   %NIJI_ARC_CTR_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy ctr\bloom_resource.gasc   %NIJI_ARC_CTR_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1


@REM ####################################
@REM  win32

%NIJI_RUBY_1_8% %BINLINKER_SCR% ^
    ../win32/shader/BloomLuminance.gfvsh ^
    ../win32/shader/BloomLuminance.gffsh ^
    ../win32/shader/BloomScale.gfvsh ^
    ../win32/shader/BloomScale.gffsh ^
    ../win32/shader/BloomBlur.gfvsh ^
    ../win32/shader/BloomBlur.gffsh ^
    ../win32/shader/BloomAdd.gfvsh ^
    ../win32/shader/BloomAdd.gffsh ^
    win32/bloom_resource_pack.pack BL 128
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

%GARC_EXE% --sort=name_up win32/bloom_resource win32/bloom_resource_pack.pack
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

@copy win32\bloom_resource_pack.h %NIJI_CONV_HEADER_NIJI_DIR%\bloom_resource\win32
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy win32\bloom_resource.gaix   %NIJI_ARC_INDEX_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy win32\bloom_resource.garc   %NIJI_ARC_WIN32_DIR%
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
@copy win32\bloom_resource.gasc   %NIJI_ARC_WIN32_DIR%
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
