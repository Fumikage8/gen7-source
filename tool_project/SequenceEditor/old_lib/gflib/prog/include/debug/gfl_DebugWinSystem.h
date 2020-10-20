//======================================================================
/**
 * @file  gfl_DebugwinSystem.h
 * @brief �f�o�b�O�E�B���h�E
 * @author  ariizumi
 * @data  11/03/29
 * 
 * 
  DebugWin�ɂ���
  
  DebugWin�ɂ̓O���[�v�A���ڂ�o�^���A�c���[��Ƀf�o�b�O���j���[��
  �쐬���邱�Ƃ��ł��܂��B
  �O���[�v�A���ڂ̍쐬�͐e�O���[�v���w�肵�A���̉��ɍ쐬����܂��B
  �폜�̓O���[�v�P�ʂł̂ݍs���A�O���[�v�̎q�̃O���[�v�A���ڂ�
  �����I�ɍ폜����܂��B
  ���ڂ͊�{�I�ɍX�V�֐���n���A�J�[�\����"�����Ă����"�X�V�֐���
  �Ă΂ꑱ���܂��B
  ���̊֐��̒��ŁA�u�{�^���������ꂽ���l��ύX����v�Ȃǂ̏�����
  ���s�����܂��B
  �g���₷������ׁA�����E�����EVEC3�̒l�ύX�̍��ڂ̒ǉ���A
  g3d::Cameta�Eg3d::Model�̍��ڕύX�̒ǉ����ꔭ�ōs����悤��
  �֐���p�ӂ��Ă��܂��B
  
  �������ׂɂ́AUpdate��Draw�̊֐����Ă�ł��炤�K�v������܂��B
  
  �g���ׂɂ͊ȈՕ`��̏������ƁA�ȈՕ`��ւ̃t�H���g�̓o�^���K�v�ł��B

 * 
 * 
 */
//======================================================================
#ifndef __GFL_DEBUGWIN_SYSTEM_H__
#define __GFL_DEBUGWIN_SYSTEM_H__
#pragma once

#include <base/gfl_Singleton.h>
#include <heap/gfl_HeapManager.h>
#include <grp/gfl_RenderTarget.h>
#include <math/gfl_Vec.h>
#include <fs/gfl_ArcFile.h>

#include <debug/gfl_DebugWinItem.h>

//��`��gfl_DebugWinTypes.h �ɂ���܂��B
//#define GFL_DEBUGWIN_USE 1

namespace gfl
{
  //�N���X��`
namespace grp
{
  class GraphicsSystem;
}
namespace ui
{
  class DeviceManager;
  class Button;
  class VectorDevice;
}

namespace str
{
  class StrBuf;
  class MsgData;
}
  
namespace debug
{
  class DebugWinItem;
  class DebugWinGroup;

#if GFL_DEBUGWIN_USE

  class DebugWinSystemCallBack
  {
  public:
    //----------------------------------------------------------------------------
    /**
     *  @brief  �f�o�b�O�E�B���h�E�p�T�E���h�Đ�
     *          gflib�ɃT�E���h����������킯�ɂ������Ȃ��̂ŁA�A�v���ɂ��C���B
     *
     *  @param  const DEBUGWIN_SOUND_TYPE �T�E���h���
     */
    //-----------------------------------------------------------------------------
    virtual void SoundCallBack( const DEBUGWIN_SOUND_TYPE type ){};
  };
  
  //----------------------------------------------------------------------------
  /**
   * @brief �f�o�b�O�E�B���h�E�̏���������
   */
  //----------------------------------------------------------------------------
  extern void DebugWin_Initialize( gfl::heap::HeapBase * heap);

  //----------------------------------------------------------------------------
  /**
   * @brief �f�o�b�O�E�B���h�E�̏I������
   */
  //----------------------------------------------------------------------------
  extern void DebugWin_Finalize( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �X�V
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_Update(void);
  //----------------------------------------------------------------------------
  /**
   *  @brief  �`��
   *
   *  @param  gfl::grp::GraphicsSystem *grpSys �O���t�B�b�N�V�X�e��
   *  @param  gfl::grp::DisplayType dispType   �\����
   *                                           �����ʂɏo���Ƃ��͕�����Ă�ŉ�����
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_Draw( gfl::grp::GraphicsSystem *grpSys , gfl::grp::DisplayType dispType );
  //----------------------------------------------------------------------------
  /**
   *  @brief  �O���[�v�̊J�`�F�b�N
   *
   *  @return bool �O���[�v���J���Ă����true
   */
  //-----------------------------------------------------------------------------
  extern bool DebugWin_IsOpen(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  DebugWin������t���[���̂ݎ擾
   *
   *  @return bool �O���[�v���J���Ă����true
   */
  //-----------------------------------------------------------------------------
  bool DebugWin_IsCloseTrigger(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �����O���[�v��
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_CloseWindow(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  �O���[�v�ǉ�
   *
   *  @param  const wchar_t *name   �\����
   *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v)
   *
   *  @return DebugWinGroup* �߂�l���g���Ďq��ǉ����Ă����Ă��������B
   */
  //-----------------------------------------------------------------------------
  extern DebugWinGroup* DebugWin_AddGroup( const wchar_t *name , DebugWinGroup *parent = NULL , heap::HeapBase *heap = NULL );
  extern DebugWinGroup* DebugWin_AddGroup( str::MsgData *msgData , u32 strId , DebugWinGroup *parent = NULL , heap::HeapBase *heap = NULL );
  //----------------------------------------------------------------------------
  /**
   *  @brief  �O���[�v�폜
   *
   *  @param  DebugWinGroup *group �폜����O���[�v
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_RemoveGroup( DebugWinGroup *parent );
  //----------------------------------------------------------------------------
  /**
   *  @brief  ���O����O���[�v�폜
   *
   *  @param  const wchar_t *name  �폜����O���[�v�̖��O
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_RemoveGroupByName( const wchar_t *name );


  //----------------------------------------------------------------------------
  /**
   *  @brief  �f�o�C�X�}�l�[�W���[�̐ݒ�
   *
   *  @param  gfl::ui::DeviceManager *devMng  �f�o�C�X�}�l�[�W���[
   *  @param  gfl::ui::Button *btn            ����Ɏg�p����{�^��
   *  @param  gfl::ui::VectorDevice *key      ����Ɏg�p����L�[
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_SetDeviceManager( gfl::ui::DeviceManager *devMng , gfl::ui::Button *btn = NULL , gfl::ui::VectorDevice *key = NULL );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �R�[���o�b�N�󂯎��N���X�̐ݒ�
   *
   *  @param  gfl::debug::DebugWinSystemCallBack *cbClass      �R�[���o�b�N���󂯎��N���X
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_SetCallbackClass( gfl::debug::DebugWinSystemCallBack *cbClass );


  //-----------------------------------------------------------------------------
  /**
   * @brief
   * @return
   */
  //-----------------------------------------------------------------------------
  extern wchar_t * DebugWin_GetWorkStr( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief  ���O����Group���擾����
   * @param name  �w�肷�閼�O
   * @return Group�ւ̃|�C���^
   */
  //-----------------------------------------------------------------------------
  extern DebugWinGroup* DebugWin_GetGroupByName( const wchar_t *name );


  //�ȉ��̓V�X�e���I�ȑ�����s�����߂̃A�N�Z�X�֐�

  ///��I���ʒu�����F�w�胏�[�N�̃|�C���^�擾
  extern gfl::math::VEC4* DebugWin_GetItemColor(void);
  ///�e�F�w�胏�[�N�̃|�C���^�擾
  extern gfl::math::VEC4* DebugWin_GetShadowColor(void);
  ///�I���ʒu�����F�w�胏�[�N�̃|�C���^�擾
  extern gfl::math::VEC4* DebugWin_GetCurrentColor(void);
  ///�h�ׂ��F�w�胏�[�N�̃|�C���^�擾
  extern gfl::math::VEC4* DebugWin_GetFillColor(void);
  ///�t�H���g�X�P�[���w�胏�[�N�̃|�C���^�擾
  extern f32* DebugWin_GetFontScale(void);
  ///�s�����w�胏�[�N�̃|�C���^�擾
  extern f32* DebugWin_GetLineHeight(void);
  ///�N���p�z�[���h�L�[�w�胏�[�N�̃|�C���^�擾
  extern int* DebugWin_GetHoldKey(void);
  ///�N���p�g���K�[�L�[�w�胏�[�N�̃|�C���^�擾
  extern int* DebugWin_GetTriggerKey(void);
  ///�ێ����Ă���UI�{�^�����ւ̃|�C���^���擾
  extern gfl::ui::Button* DebugWin_GetUiButton(void);

  

#else  //GFL_DEBUGWIN_USE

/*
  #define DebugWin_Update() ((void)0)
  #define DebugWin_Draw(grpSys,dispType) ((void)0)
  #define DebugWin_IsOpen() (false)
  #define DebugWin_AddGroup(name,parent) (NULL)
  #define DebugWin_RemoveGroup(parent) ((void)0)
  #define DebugWin_RemoveGroupByName(name) ((void)0)
*/

  extern void DebugWin_Update(void);
  extern void DebugWin_Draw(...);
  extern bool DebugWin_IsOpen(void);
  extern bool DebugWin_IsCloseTrigger(void);
  extern void DebugWin_CloseWindow(void);
  extern DebugWinGroup* DebugWin_AddGroup(...);
  extern DebugWinGroup* DebugWin_AddGroupMsg(...);
  extern void DebugWin_RemoveGroup(...);
  extern void DebugWin_RemoveGroupByName(...);
#endif //GFL_DEBUGWIN_USE

}//namespace debug
}//namespace gfl




#endif  //__GFL_DEBUGWIN_SYSTEM_H__
