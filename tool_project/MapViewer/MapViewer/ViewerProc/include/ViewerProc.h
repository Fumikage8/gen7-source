//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldProc.h
 *	@brief  �t�B�[���h�v���Z�X
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
///#if !defined( __VIEWER_PROC_H__ )
///#define __VIEWER_PROC_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_List.h>

// niji
#include "GameSystem/include/GameProc.h"
#include "System/include/GflvUse.h"


#include "ViewerProc/include/FieldRo/Fieldmap.h"

GFL_NAMESPACE_BEGIN( Field );

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
class ViewerProc : public GameSystem::GameProc
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( ViewerProc );



public:
  /**------------------------------------------------------------------
   *      ���J��`
   ------------------------------------------------------------------*/


  /**------------------------------------------------------------------
   *      ���J�֐�
   ------------------------------------------------------------------*/

public:
  /**------------------------------------------------------------------
   *      GameProc���z�֐�
   ------------------------------------------------------------------*/
  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  ViewerProc( void/*FieldmapDesc* desc*/ );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~ViewerProc( void );

  /**
  * @brief   PROC�̏��������s���֐�
  * @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����UpdateFunc�ɐi��
  */
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	  PROC�̃��C���X�V���s���֐�
  * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
  */
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROC�̕`�揈��(������)
  * @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
  * �o�^����PROC�ɂ��A�f�B�X�v���C�o�b�t�@�Ɉˑ����Ȃ��`�揈�������s���܂��B
  * ��) ���̎����̕`��R�}���h�̃Z�[�u�A�I�t�X�N���[���o�b�t�@�ւ̕`��
  */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROC�̕`�揈��
  * @param pManager   �v���Z�X�Ǘ��}�l�[�W���[
  * @param displayNo  �`��Ώۉ��
  * �o�^����PROC�ɂ��A�t���[���o�b�t�@�ւ̕`������s���܂��B
  * �f�B�X�v���C�o�b�t�@�̐؂�ւ��A�f�B�X�v���C�o�b�t�@�ւ̓]���͌Ăяo�������s���܂��B
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  /**
  * @brief   PROC�̃��C�����s���֐�
  *  1/30�t���[���̇A�t���[���ڂ̏����ł��B
  *  1/60�t���[�����쎞�ɂ́A�g���܂���B
  * @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
  */
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief   PROC�̏I�����s���֐�
  * @return  Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤���ŏI������Ɖ��߂���
  */
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
#if 0
  //------------------------------------------------------------------
  /**
   * @brief   PROC�̃C�x���g�N�����m����
   *
   *  �ȉ��̃^�C�~���O�ōs���ׂ��A�C�x���g�N������
   *  �@GameProc->EventCheckFunc
   *  �@EventManager->Main
   *  �@GameProc->Main
   */
  //------------------------------------------------------------------
  virtual void EventCheckFunc(GameSystem::GameManager* pManager);
#endif

  /**
  * @brief   FrameMode�̎擾
  * @retval   FrameMode
  *
  * �t���[�����[�h��؂�ւ���Ƃ��́A�߂�l��ς��Ă��������B
  */
  ///virtual GameSystem::FrameMode GetFrameMode( void ) const { return GameSystem::FRAMEMODE_30; }



private:
  Fieldmap* m_pFieldmap;
  ///FieldmapDesc m_desc;
};

GFL_NAMESPACE_END( Field );

///#endif	// __VIEWER_PROC_H__

