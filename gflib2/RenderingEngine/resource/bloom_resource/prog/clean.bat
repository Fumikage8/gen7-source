@echo off

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

del %NIJI_CONV_HEADER_NIJI_DIR%\bloom_resource\ctr\bloom_resource_pack.h
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

del %NIJI_ARC_INDEX_DIR%\bloom_resource.gaix
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_CTR_DIR%\bloom_resource.garc
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_CTR_DIR%\bloom_resource.gasc
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1


@REM ####################################
@REM  win32

del %NIJI_CONV_HEADER_NIJI_DIR%\bloom_resource\win32\bloom_resource_pack.h
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1

del %NIJI_ARC_INDEX_DIR%\bloom_resource.gaix
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_WIN32_DIR%\bloom_resource.garc
@if not %ERRORLEVEL% == 0   SET LOCAL_ERRORLEVEL=1
del %NIJI_ARC_WIN32_DIR%\bloom_resource.gasc
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
