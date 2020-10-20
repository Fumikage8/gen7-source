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
#ifndef __GFL_APPLET_H__
#define __GFL_APPLET_H__

namespace gfl
{
namespace base
{
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�N���O�Z�[�u�����A�@�ナ�J�o�[�����@�C���^�[�t�F�[�X
   */
  //-----------------------------------------------------------------------------
  class AppletInterface
  {
  public:
    virtual void SaveApplication( void ) = 0;  ///< �A�v���P�[�V�����Z�[�u����
    virtual void RecoverApplication( void ) = 0;  ///< �A�v���P�[�V�������A����
    virtual void FinalizeApplication( void ) = 0;  ///< �A�v���P�[�V�����I������
    virtual bool IsSleepEnable( void ) = 0; ///< �A�v���P�[�V�������ŃX���[�v�������Ă��邩�H
  };


  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�v���b�g�N���O��Ǘ��֐��Q
   */
  //-----------------------------------------------------------------------------
  class Applet
  {
  public:
    //----------------------------------------------------------------------------
    /**
     *  @brief  �C���^�[�t�F�[�X�̐ݒ�
     */
    //-----------------------------------------------------------------------------
    static void SetInterface( AppletInterface * pInterface );

    //----------------------------------------------------------------------------
    /**
     *  @brief  �A�v���b�g�R�[���O����
     *
     *  @note �����ŁAAppletInterface->SaveApplication()���R�[��
     */
    //-----------------------------------------------------------------------------
    static void AppletCallBefore( void );

    //----------------------------------------------------------------------------
    /**
     *  @brief  �A�v���b�g�R�[���㏈���@�i�A�v���P�[�V�����ɕ��A����Ƃ��̂ݎg�p����j
     *
     *  @note �����ŁAAppletInterface->RecoverApplication()���R�[��
     */
    //-----------------------------------------------------------------------------
    static void AppletCallAfter( void );

    //----------------------------------------------------------------------------
    /**
     *  @brief  �A�v���b�g�R�[���I�������@�i�A�v���I���O�ɌĂ΂��j
     *
     *  @note �����ŁAAppletInterface->FinalizeApplication()���R�[��
     */
    //-----------------------------------------------------------------------------
    static void AppletCallFinalize( void );

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
    static nn::applet::AppletWakeupState ProcessHomeButton( void );



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
    static void ProcessPowerButton( void );

  private:

    // �Z�[�u�@�E�@���J�o�[�@�C���^�[�t�F�[�X
    static AppletInterface * m_pInterface;
  };
}
}

#endif // __GFL_APPLET_H__
