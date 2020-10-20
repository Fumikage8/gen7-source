@echo off


@REM ############################################################################
@REM  ���dCALL�h�~
if defined GFL2_RENDERINGENGINE_RESOURCE_BLOOM_RESOURCE_MACRO_SETTING_CALL (
  @REM CALL�ς�
  exit /b 0
) else (
  @REM ����CALL
  SET GFL2_RENDERINGENGINE_RESOURCE_BLOOM_RESOURCE_MACRO_SETTING_CALL=true
)


@REM ############################################################################
@REM  ���ݒ�
CALL %NIJI_ENVIRONMENT_ROOT%/macro_setting.bat


@REM ############################################################################
@REM  ���[�J���ϐ�
SETLOCAL
SET LOCAL_ERRORLEVEL=0


@REM ############################################################################
@REM  ���s

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
@REM  �I��
@if not %LOCAL_ERRORLEVEL% == 0 (
  echo ERROR: �G���[���������܂����B
) else (
  echo SUCCESS: ����ɏI�����܂����B
)

pause

@REM  exit�̑O�ɈÖق�ENDLOCAL������܂�
@if not %LOCAL_ERRORLEVEL% == 0 (
  exit /b 1
) else (
  exit /b 0
)
