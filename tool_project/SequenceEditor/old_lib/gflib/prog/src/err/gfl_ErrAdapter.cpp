//=============================================================================
/**
 * @file   gfl_ErrAdapter.cpp
 * @brief  �G���[EULA�A�v���b�g��K�؂ɏ�������N���X
 * @author k.ohno
 * @date   2012.10.23
 */
//=============================================================================

#include <gflib.h>
#include "err/gfl_ErrAdapter.h"
#include <base/gfl_Applet.h>


GFL_NAMESPACE_BEGIN(gfl)
GFL_NAMESPACE_BEGIN(err)

nn::erreula::Parameter ErrAdapter::mEreParam;  //�n���p�����[�^ 1KB�ȏ゠��̂Œ�`

//------------------------------------------------------------------
/**
 * @brief   �C�V����nn:Result�Ɋi�[���ꂽ�G���[���Z�b�g����
 * @param   errorCode  �G���[�R�[�h
 * @param   errorType  �G���[�^�C�v
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(u32 errorCode, nn::erreula::ErrorType errorType)
{

  // �ݒ�\���̏�����
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = errorType;
  mEreParam.config.errorCode = errorCode;
  
  return _ErrorDispDirectMain(&mEreParam);
}

//------------------------------------------------------------------
/**
 * @brief   �C�V����nn:Result�Ɋi�[���ꂽ�G���[���Z�b�g����
 * @param   errorCode  �G���[�R�[�h
 * @param   errorType  �G���[�^�C�v
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(gfl::str::StrBuf* pBuf)
{

  // �ݒ�\���̏�����
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  pBuf->PutStr(mEreParam.config.errorText,nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}

#if GFL_DEBUG

//------------------------------------------------------------------
/**
 * @brief   �C�V����nn:Result�Ɋi�[���ꂽ�G���[���Z�b�g����
 * @param   errorCode  �G���[�R�[�h
 * @param   errorType  �G���[�^�C�v
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(const wchar_t* pMessage)
{
  // �ݒ�\���̏�����
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  memcpy(mEreParam.config.errorText , pMessage, nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}


//------------------------------------------------------------------
/**
 * @brief   �C�V����nn:Result�Ɋi�[���ꂽ�G���[���Z�b�g����
 *          mbstowcs�ɂ͌��E������A�������\������Ȃ���������܂��B wchar_t�Ȃ�o�܂�
 * @param   errorCode  �G���[�R�[�h
 * @param   errorType  �G���[�^�C�v
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::ErrorDisp(const char* pMessage)
{
  // �ݒ�\���̏�����
  nn::erreula::InitializeConfig( &mEreParam.config );

  mEreParam.config.errorType = nn::erreula::ERROR_TYPE_ERROR_TEXT;
  mbstowcs(mEreParam.config.errorText , pMessage, nn::erreula::ERROR_TEXT_LENGTH_MAX);
  return _ErrorDispDirectMain(&mEreParam);
}

void ErrAdapter::ErrorDispChar(const char* pMessage)
{
  ErrorDisp(pMessage);
}




#endif //GFL_DEBUG

//------------------------------------------------------------------
/**
 * @brief   �C�V����nn:Result�Ɋi�[���ꂽ�G���[���Z�b�g����
 * @param   errorCode  �G���[�R�[�h
 * @param   errorType  �G���[�^�C�v
 */
//------------------------------------------------------------------
nn::erreula::CTR::ReturnCode ErrAdapter::_ErrorDispDirectMain(nn::erreula::Parameter* ere_param)
{
  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�R�[���O����
   */
  //-----------------------------------------------------------------------------
  base::Applet::AppletCallBefore();

  gfl::cfg::Config config;
  config.Initialize();

  nn::applet::AppletWakeupState wstate;

  // �\�t�g���Z�b�g��HOME�{�^�����֎~
  ere_param->config.upperScreenFlag = nn::erreula::UPPER_SCREEN_NORMAL;
  ere_param->config.homeButton    = false;
  ere_param->config.softwareReset = false;
  ere_param->config.appJump = false;

  nn::erreula::StartErrEulaApplet( &wstate, ere_param );

  if(nn::applet::IsExpectedToCloseApplication()){
    // �I���ւ̑J�ڂ́A�\�t�g�E�F�A�ɔC����i��΂ɗp�ӂ���Ă���j
    /*
    {
      nn::applet::PrepareToCloseApplication();
      nn::applet::CloseApplication();
      // �����ɂ͓��B���܂���
    }
    */
    return ere_param->config.returnCode;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�R�[���㏈��
   */
  //-----------------------------------------------------------------------------
  base::Applet::AppletCallAfter();

  config.Finalize();

  return ere_param->config.returnCode;
}


GFL_NAMESPACE_END(err)
GFL_NAMESPACE_END(gfl)



