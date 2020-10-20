//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Applet.h
 *	@brief  �A�v���b�g�@�Ǘ�
 *	@author tomoya takahashi
 *	@date		2013.02.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <base/gfl_Applet.h>

#include <std/gfl_Date.h>

#include <grp/gfl_GraphicsSystem.h>

namespace gfl
{
namespace base
{

  /// �A�v���b�g�C���^�[�t�F�[�X
  AppletInterface * Applet::m_pInterface = NULL;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �C���^�[�t�F�[�X�̐ݒ�
   */
  //-----------------------------------------------------------------------------
  void Applet::SetInterface( AppletInterface * pInterface )
  {
    m_pInterface = pInterface;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�R�[���O����
   *
   *  @note �����ŁAAppletInterface->SaveApplication()���R�[��
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallBefore( void )
  {
    if( m_pInterface ){ m_pInterface->SaveApplication(); }

    //------------------------
    //SDK4.0����R�}���h���X�g�����������邱�Ƃ��K�{�ɂȂ�܂����I
    //------------------------
    //���s���̃R�}���h���I��点��B
    nngxWaitCmdlistDone();
    nngxWaitVSync(NN_GX_DISPLAY_BOTH);
    //------------------------
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�R�[���㏈���@�i�A�v���P�[�V�����ɕ��A����Ƃ��̂ݎg�p����j
   *
   *  @note �����ŁAAppletInterface->RecoverApplication()���R�[��
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallAfter( void )
  {
    grp::GraphicsSystem * pGraphicsSystem = GFL_SINGLETON_INSTANCE(gfl::grp::GraphicsSystem);

    nngxWaitCmdlistDone();  ///< ���s������������B
    GFL_GRPGL_ERROR_CHECK();

    /* gx ���C�u�������g�p���Ă���ꍇ */
    nngxUpdateState(NN_GX_STATE_ALL);
    /* GD ���C�u�������g�p���Ă���ꍇ */
    nn::gd::System::ForceDirty(nn::gd::System::MODULE_ALL);

    //�������ݐ�̃R�}���h���X�g��ݒ肷��B
    if( pGraphicsSystem )
    {
      pGraphicsSystem->BindCurrentCommandList();
    }
    //------------------------

    //------------------------
    //���ԕ��A
    //------------------------
    gfl::std::DateTime::Update(true);

    if( m_pInterface ){ m_pInterface->RecoverApplication(); }
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�R�[���I�������@�i�A�v���I���O�ɌĂ΂��j
   *
   *  @note �����ŁAAppletInterface->FinalizeApplication()���R�[��
   */
  //-----------------------------------------------------------------------------
  void Applet::AppletCallFinalize( void )
  {
    if( m_pInterface ){ m_pInterface->FinalizeApplication(); }
  }
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  Home�{�^���v���Z�X�̎��s
   *
   *  @note ���L�̏��������s���܂��B
   *        IsExpectedToProcessHomeButton()�`�F�b�N 
   *        DisableSleep()
   *        AppletCallBefore()
   *        ProcessHomeButton()
   *        nn::applet::WaitForStarting()
   *        nn::applet::IsExpectedToCloseApplication()�`�F�b�N  -> �A�v���P�[�V�����I��
   *        AppletCallAfter()
   *        EnableSleep()
   *        
   *  @note �A�v���P�[�V�����I���ɑJ�ڂ����ꍇ�A���̊֐�����͔����Ă��܂���B
   */
  //-----------------------------------------------------------------------------
  nn::applet::AppletWakeupState Applet::ProcessHomeButton( void )
  {
    nn::applet::AppletWakeupState r = nn::applet::WAKEUP_STATE_MAX;
    // HOME�{�^������
    if( nn::applet::IsExpectedToProcessHomeButton() ){

      //---------------------
      // �A�v���b�g���s�O����
      //=====================
      AppletCallBefore();

      bool is_sleep = true;
      if( m_pInterface ){
        is_sleep = m_pInterface->IsSleepEnable();
      }
      if( is_sleep ){ nn::applet::DisableSleep(true); }

      nn::applet::ProcessHomeButton();
      r = nn::applet::WaitForStarting();

      if( nn::applet::IsExpectedToCloseApplication() )
      { 
        nn::applet::CTR::PrepareToCloseApplication();
        nn::applet::CTR::CloseApplication();
        // �����ɂ͓��B���܂���
      }

      if( is_sleep ){ nn::applet::EnableSleep(true); }

      //---------------------
      // �A�v���b�g���s�㏈��
      //=====================
      AppletCallAfter();
    }

    return r;
  }


  //----------------------------------------------------------------------------
  /**
   *  @brief  �d���{�^���v���Z�X�̎��s
   *
   *  @note ���L�̏��������s���܂��B
   *        IsExpectedToProcessPowerButton()()�`�F�b�N 
   *        DisableSleep()
   *        AppletCallBefore()
   *        ProcessPowerButton()
   *        nn::applet::WaitForStarting()
   *        nn::applet::IsExpectedToCloseApplication()�`�F�b�N  -> �A�v���P�[�V�����I��
   *        
   *  @note ���̊֐�����͔����Ă��܂���B
   */
  //-----------------------------------------------------------------------------
  void Applet::ProcessPowerButton( void )
  {
    if( nn::applet::IsExpectedToProcessPowerButton() )
    {
      //---------------------
      // �A�v���b�g���s�O����
      //=====================
      AppletCallBefore();

      bool is_sleep = true;
      if( m_pInterface ){
        is_sleep = m_pInterface->IsSleepEnable();
      }
      if( is_sleep ){ nn::applet::DisableSleep(true); }


      nn::applet::ProcessPowerButton();
      nn::applet::AppletWakeupState r = nn::applet::WaitForStarting();
      NN_UNUSED_VAR(r);

      if( nn::applet::IsExpectedToCloseApplication() )
      { 
        nn::applet::CTR::PrepareToCloseApplication();
        nn::applet::CTR::CloseApplication();
        // �����ɂ͓��B���܂���
      }

      // SDK 5.0����
      // �����ɂ͓��B���邱�Ƃ�����܂���B
      // �ꉞ�R�[�f�B���O���Ă����܂��B

      if( is_sleep ){ nn::applet::EnableSleep(true); }

      //---------------------
      // �A�v���b�g���s�㏈��
      //=====================
      AppletCallAfter();
    }
  }

}
}



